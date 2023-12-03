#include "ESceneAIMapTools.h"
// chunks
#define AIMAP_VERSION  				0x0002

#define AIMAP_CHUNK_VERSION			0x0001       
#define AIMAP_CHUNK_FLAGS			0x0002
#define AIMAP_CHUNK_BOX				0x0003
#define AIMAP_CHUNK_PARAMS			0x0004
#define AIMAP_CHUNK_NODES			0x0006
#define AIMAP_CHUNK_SNAP_OBJECTS	0x0007
#define AIMAP_CHUNK_INTERNAL_DATA	0x0008
#define AIMAP_CHUNK_INTERNAL_DATA2	0x0009

static const DWORD InvalidNode = (1 << 24) - 1;

ESceneAIMapTool::ESceneAIMapTool() :FXRaySceneToolBase(ERBMKSceneObjectType::AIMap)
{

}

ESceneAIMapTool::~ESceneAIMapTool()
{
	for (SAINode* Node : m_Nodes)
	{
		delete			Node;
	}
}


bool ESceneAIMapTool::LoadStream(IReader&F)
{
	inherited::LoadStream(F);

	u16 version = 0;

	R_ASSERT(F.r_chunk(AIMAP_CHUNK_VERSION, &version));
	if (version != AIMAP_VERSION) {
		ELog.DlgMsg(mtError, "AIMap: Unsupported version.");
		return false;
	}

	R_ASSERT(F.find_chunk(AIMAP_CHUNK_FLAGS));
	F.r(&m_Flags, sizeof(m_Flags));

	R_ASSERT(F.find_chunk(AIMAP_CHUNK_BOX));
	F.r(&m_AIBBox, sizeof(m_AIBBox));

	R_ASSERT(F.find_chunk(AIMAP_CHUNK_PARAMS));
	F.r(&m_Params, sizeof(m_Params));

	R_ASSERT(F.find_chunk(AIMAP_CHUNK_NODES));
	m_Nodes.resize(F.r_u32());
	for (AINodeIt it = m_Nodes.begin(); it != m_Nodes.end(); it++) {
		*it = xr_new<SAINode>();
		(*it)->LoadStream(F, this);
	}
	DenumerateNodes();

	return true;
}

void ESceneAIMapTool::UnpackPosition(Fvector& Pdest, const SNodePositionOld& Psrc, Fbox& bb, SAIParams& params)
{
	Pdest.x = float(Psrc.x) * params.fPatchSize;
	Pdest.y = (float(Psrc.y) / 65535) * (bb.max.y - bb.min.y) + bb.min.y;
	Pdest.z = float(Psrc.z) * params.fPatchSize;
}

u32 ESceneAIMapTool::UnpackLink(u32& L)
{
	return L&0x00FFFFFF;
}

void ESceneAIMapTool::DenumerateNodes()
{
	u32 cnt = m_Nodes.size();
	int32 Index =0;
	for (AINodeIt it = m_Nodes.begin(); it != m_Nodes.end(); it++) 
	{
		if (!(((static_cast<u32>(reinterpret_cast<size_t>((*it)->n1)) < cnt) || (static_cast<u32>(reinterpret_cast<size_t>((*it)->n1)) == InvalidNode)) &&
			((static_cast<u32>(reinterpret_cast<size_t>((*it)->n2)) < cnt) || (static_cast<u32>(reinterpret_cast<size_t>((*it)->n2)) == InvalidNode)) &&
			((static_cast<u32>(reinterpret_cast<size_t>((*it)->n3)) < cnt) || (static_cast<u32>(reinterpret_cast<size_t>((*it)->n3)) == InvalidNode)) &&
			((static_cast<u32>(reinterpret_cast<size_t>((*it)->n4)) < cnt) || (static_cast<u32>(reinterpret_cast<size_t>((*it)->n4)) == InvalidNode)))) {
			ELog.Msg(mtError, "Node: has wrong link [%3.2f, %3.2f, %3.2f], {%p,%p,%p,%p}", VPUSH((*it)->Pos), (*it)->n1, (*it)->n2, (*it)->n3, (*it)->n4);
			(*it)->n1 = 0;
			(*it)->n2 = 0;
			(*it)->n3 = 0;
			(*it)->n4 = 0;
			continue;
		}
		//                     ,"AINode: Wrong link found.");
		(*it)->n1 = (static_cast<u32>(reinterpret_cast<size_t>((*it)->n1)) == InvalidNode) ? 0 : m_Nodes[static_cast<u32>(reinterpret_cast<size_t>((*it)->n1))];
		(*it)->n2 = (static_cast<u32>(reinterpret_cast<size_t>((*it)->n2)) == InvalidNode) ? 0 : m_Nodes[static_cast<u32>(reinterpret_cast<size_t>((*it)->n2))];
		(*it)->n3 = (static_cast<u32>(reinterpret_cast<size_t>((*it)->n3)) == InvalidNode) ? 0 : m_Nodes[static_cast<u32>(reinterpret_cast<size_t>((*it)->n3))];
		(*it)->n4 = (static_cast<u32>(reinterpret_cast<size_t>((*it)->n4)) == InvalidNode) ? 0 : m_Nodes[static_cast<u32>(reinterpret_cast<size_t>((*it)->n4))];
		(*it)->idx =  Index++;
	}
}

bool ESceneAIMapTool::LoadLTX(CInifile&)
{
	return false;
}

bool ESceneAIMapTool::LoadSelection(IReader&)
{
	return false;
}

void ESceneAIMapTool::Clear()
{

}

void SAINode::LoadStream(IReader&F, ESceneAIMapTool* tools)
{
	u32 			id;
	u16 			pl;
	SNodePositionOld 	np;
	F.r(&id, 3); 			n1 = (SAINode*)tools->UnpackLink(id);
	F.r(&id, 3); 			n2 = (SAINode*)tools->UnpackLink(id);
	F.r(&id, 3); 			n3 = (SAINode*)tools->UnpackLink(id);
	F.r(&id, 3); 			n4 = (SAINode*)tools->UnpackLink(id);
	pl = F.r_u16(); 		pvDecompress(Plane.n, pl);
	F.r(&np, sizeof(np)); 	tools->UnpackPosition(Pos, np, tools->m_AIBBox, tools->m_Params);
	Plane.build(Pos, Plane.n);
	flags.assign(F.r_u8());
}

