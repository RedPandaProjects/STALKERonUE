#include "XRaySkeletonX.h"
#include "../Visual/XRayKinematicsLegacy.h"
#include "../Source/XrSound/cl_intersect.h"

void XRaySkeletonX::_Copy(XRaySkeletonX* B)
{
	ParentKinematics = NULL;
	ChildIDX = B->ChildIDX;
	Vertices1W = B->Vertices1W;
	Vertices2W = B->Vertices2W;
	Vertices3W = B->Vertices3W;
	Vertices4W = B->Vertices4W;

	cache_DiscardID = B->cache_DiscardID;
	cache_vCount = B->cache_vCount;
	cache_vOffset = B->cache_vOffset;
	RenderMode = B->RenderMode;
	RMS_boneid = B->RMS_boneid;

	m_Indices = B->m_Indices;
}
void XRaySkeletonX::AfterLoad(XRayKinematicsLegacy* parent, u16 child_idx)
{
	SetParent(parent);
	ChildIDX = child_idx;
}
void XRaySkeletonX::_Load(const char* N, IReader* data, size_t& dwVertCount)
{
	xr_vector<u16>			bids;

	check(data->find_chunk(OGF_VERTICES));

	size_t								dwVertType;
	
	dwVertType = data->r_u32();
	dwVertCount = data->r_u32();

	Render->shader_option_skinning(-1);

	switch (dwVertType)
	{
	case OGF_VERTEXFORMAT_FVF_1L: // 1-Link
	case 1:
	{
		RenderMode = RM_SKINNING_1B;
	}
	break;
	case OGF_VERTEXFORMAT_FVF_2L: // 2-Link
	case 2:
	{
		RenderMode = RM_SKINNING_2B;
	}break;
	case OGF_VERTEXFORMAT_FVF_3L: // 3-Link
	case 3:
	{
		RenderMode = RM_SKINNING_3B;
	}break;
	case OGF_VERTEXFORMAT_FVF_4L: // 4-Link
	case 4:
	{
		RenderMode = RM_SKINNING_4B;
	}break;
	default:
		Debug.fatal(DEBUG_INFO, "Invalid vertex type in skinned model '%s'", N);
		break;
	}
}


inline void 	get_pos_bones(const vertBoned1W& v, Fvector& p, XRayKinematicsLegacy* Parent)
{
	const Fmatrix& xform = Parent->LL_GetTransform_R((u16)v.matrix);
	xform.transform_tiny(p, v.P);
}

inline void 	get_pos_bones(const vertBoned2W& vert, Fvector& p, XRayKinematicsLegacy* Parent)
{
	Fvector		P0, P1;

	const Fmatrix& xform0 = Parent->LL_GetTransform_R(vert.matrix0);
	const Fmatrix& xform1 = Parent->LL_GetTransform_R(vert.matrix1);
	xform0.transform_tiny(P0, vert.P);
	xform1.transform_tiny(P1, vert.P);
	p.lerp(P0, P1, vert.w);
}

inline void 	get_pos_bones(const vertBoned3W& vert, Fvector& p, XRayKinematicsLegacy* Parent)
{
	const Fmatrix& M0 = Parent->LL_GetTransform_R(vert.m[0]);
	const Fmatrix& M1 = Parent->LL_GetTransform_R(vert.m[1]);
	const Fmatrix& M2 = Parent->LL_GetTransform_R(vert.m[2]);

	Fvector	P0, P1, P2;
	M0.transform_tiny(P0, vert.P); P0.mul(vert.w[0]);
	M1.transform_tiny(P1, vert.P); P1.mul(vert.w[1]);
	M2.transform_tiny(P2, vert.P); P2.mul(1.0f - vert.w[0] - vert.w[1]);

	p = P0;
	p.add(P1);
	p.add(P2);
}
inline void 	get_pos_bones(const vertBoned4W& vert, Fvector& p, XRayKinematicsLegacy* Parent)
{
	const Fmatrix& M0 =Parent->LL_GetTransform_R(vert.m[0]);
	const Fmatrix& M1 = Parent->LL_GetTransform_R(vert.m[1]);
	const Fmatrix& M2 = Parent->LL_GetTransform_R(vert.m[2]);
	const Fmatrix& M3 = Parent->LL_GetTransform_R(vert.m[3]);

	Fvector	P0, P1, P2, P3;
	M0.transform_tiny(P0, vert.P); P0.mul(vert.w[0]);
	M1.transform_tiny(P1, vert.P); P1.mul(vert.w[1]);
	M2.transform_tiny(P2, vert.P); P2.mul(vert.w[2]);
	M3.transform_tiny(P3, vert.P); P3.mul(1.0f - vert.w[0] - vert.w[1] - vert.w[2]);

	p = P0;
	p.add(P1);
	p.add(P2);
	p.add(P3);
}
template<typename T_vertex, typename T_buffer >
inline BOOL pick_bone(T_buffer vertices, XRayKinematicsLegacy* Parent, IKinematics::pick_result& r, float dist, const Fvector& S, const Fvector& D, u16* indices, CBoneData::FacesVec& faces)
{


	for (auto it = faces.begin(); it != faces.end(); it++) {

		u32 idx = (*it) * 3;
		for (u32 k = 0; k < 3; k++) {
			T_vertex& vert = vertices[indices[idx + k]];
			get_pos_bones(vert, r.tri[k], Parent);
		}
		float u, v;
		r.dist = flt_max;
		if (CDB::TestRayTri(S, D, r.tri, u, v, r.dist, true) && (r.dist < dist)) {
			r.normal.mknormal(r.tri[0], r.tri[1], r.tri[2]);
			return TRUE;
		};
	}
	return FALSE;
}
BOOL XRaySkeletonX::_PickBoneSoft1W(IKinematics::pick_result& r, float dist, const Fvector& S, const Fvector& D, u16* indices, CBoneData::FacesVec& faces)
{
	return pick_bone<vertBoned1W>(Vertices1W, ParentKinematics, r, dist, S, D, indices, faces);
}

BOOL XRaySkeletonX::_PickBoneSoft2W(IKinematics::pick_result& r, float dist, const Fvector& S, const Fvector& D, u16* indices, CBoneData::FacesVec& faces)
{
	return pick_bone<vertBoned2W>(Vertices2W, ParentKinematics, r, dist, S, D, indices, faces);
}

BOOL XRaySkeletonX::_PickBoneSoft3W(IKinematics::pick_result& r, float dist, const Fvector& S, const Fvector& D, u16* indices, CBoneData::FacesVec& faces)
{
	return pick_bone<vertBoned3W>(Vertices3W, ParentKinematics, r, dist, S, D, indices, faces);
}

BOOL XRaySkeletonX::_PickBoneSoft4W(IKinematics::pick_result& r, float dist, const Fvector& S, const Fvector& D, u16* indices, CBoneData::FacesVec& faces)
{
	return pick_bone<vertBoned4W>(Vertices4W, ParentKinematics, r, dist, S, D, indices, faces);
}

void XRaySkeletonX::_DuplicateIndices(const char* N, IReader* data)
{
	checkSlow(!data->find_chunk(OGF_ICONTAINER));
	//	Index buffer replica since we can't read from index buffer in DX10
	//ref_smem<u16>			Indices;
	check(data->find_chunk(OGF_INDICES));
	u32 iCount = data->r_u32();

	u32 size = iCount * 2;
	u32 crc = crc32(data->pointer(), size);
	m_Indices.create(crc, iCount, (u16*)data->pointer());
}
