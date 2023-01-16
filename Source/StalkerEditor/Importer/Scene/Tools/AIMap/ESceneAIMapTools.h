#pragma once
#include "AIMapExport.h"
#pragma pack(push,1)
struct SAINode					// definition of "patch" or "node"
{
	union {
		struct {
			SAINode* n1;		// Left
			SAINode* n2;		// Forward
			SAINode* n3;		// Right
			SAINode* n4;		// Backward
		};
		SAINode* n[4];
	};
	Fplane		Plane;			// plane of patch										
	Fvector		Pos;			// position of patch center							
	Fvector		SavePos;			// position of patch center									

	enum {
		flSelected = (1 << 0),
		flHLSelected = (1 << 1),
		//    	flHide		= (1<<2), 	// obsolette

		flN1 = (1 << 4),
		flN2 = (1 << 5),
		flN3 = (1 << 6),
		flN4 = (1 << 7),
	};
	Flags8		flags;
	u32			idx;

	SAINode() { n1 = n2 = n3 = n4 = 0; idx = 0; flags.zero(); }
	SAINode* nLeft() { return n1; }
	SAINode* nForward() { return n2; }
	SAINode* nRight() { return n3; }
	SAINode* nBack() { return n4; }

	int			Links()	const { int cnt = 0; for (int k = 0; k < 4; k++) if (n[k]) cnt++; return cnt; }

	void   		LoadStream(IReader&,class ESceneAIMapTool*);
};
#pragma pack(pop)

DEFINE_VECTOR(SAINode*, AINodeVec, AINodeIt);

class ESceneAIMapTool : public ESceneToolBase
{
	typedef ESceneToolBase inherited;
public:
	ESceneAIMapTool();
	~ESceneAIMapTool() override;
	// definition
	LPCSTR			ClassName() override { return "ai_map"; }
	// IO

	bool   				LoadStream(IReader&) override;
	bool 				can_use_inifile() override { return false; }
	void 				UnpackPosition(Fvector& Pdest, const SNodePositionOld& Psrc, Fbox& bb, SAIParams& params);
	u32 				UnpackLink(u32& L);
	void				DenumerateNodes();
	bool				LoadLTX(CInifile&) override;
	bool				LoadSelection(IReader&) override;
	void				Clear() override;
	Flags32				m_Flags;
	Fbox				m_AIBBox;
	AINodeVec			m_Nodes;
	SAIParams			m_Params;




};