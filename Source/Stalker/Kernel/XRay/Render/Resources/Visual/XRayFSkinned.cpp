#include "XRayFSkinned.h"
#include "XRayKinematicsLegacy.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/EnnumerateVertices.h"
THIRD_PARTY_INCLUDES_END
static shared_str sbones_array;

#pragma pack(push, 1)

float u_P(s16 v)
{
	return float(v) / (32767.f / 12.f);
}

s16 q_P(float v)
{
	int _v = clampr(iFloor(v * (32767.f / 12.f)), -32768, 32767);
	return s16(_v);
}

u8 q_N(float v)
{
	int _v =  clampr(iFloor((v + 1.f) * 127.5f), 0, 255);
	return u8(_v);
}

s16 q_tc(float v)
{
	int _v =  clampr(iFloor(v * (32767.f / 16.f)), -32768, 32767);
	return s16(_v);
}

#ifdef _DEBUG
float errN(Fvector3 v, u8* qv)
{
	Fvector3 uv;
	uv.set(float(qv[0]), float(qv[1]), float(qv[2])).div(255.f).mul(2.f).sub(1.f);
	uv.normalize();
	return v.dotproduct(uv);
}
#else
float errN(Fvector3 v, u8* qv)
{
	return 0;
}
#endif


struct vertHW_1W
{
	float _P[4];
	u32 _N_I;
	u32 _T;
	u32 _B;
	float _tc[2];

	void set(Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, Fvector2& tc, int index)
	{
		N.normalize_safe();
		T.normalize_safe();
		B.normalize_safe();

		_P[0] = P.x;
		_P[1] = P.y;
		_P[2] = P.z;
		_P[3] = 1.f;

		_N_I = color_rgba(q_N(N.x), q_N(N.y), q_N(N.z), u8(index));
		_T = color_rgba(q_N(T.x), q_N(T.y), q_N(T.z), 0);
		_B = color_rgba(q_N(B.x), q_N(B.y), q_N(B.z), 0);

		_tc[0] = tc.x;
		_tc[1] = tc.y;
	}

	u16 get_bone() const
	{
		return (u16)color_get_A(_N_I) / 3;
	}

	/*void get_pos_bones(Fvector& p, XRayKinematicsLegacy* Parent) const
	{
		const Fmatrix& xform = Parent->LL_GetBoneInstance(get_bone()).mRenderTransform;
		get_pos(p);
		xform.transform_tiny(p);
	}*/

	void get_pos(Fvector& p) const
	{
		p.x = _P[0];
		p.y = _P[1];
		p.z = _P[2];
	}
};


struct vertHW_2W
{
	float _P[4];
	u32 _N_w;
	u32 _T;
	u32 _B;
	float _tc_i[4];

	void set(Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, Fvector2& tc, int index0, int index1, float w)
	{
		N.normalize_safe();
		T.normalize_safe();
		B.normalize_safe();

		_P[0] = P.x;
		_P[1] = P.y;
		_P[2] = P.z;
		_P[3] = 1.f;

		_N_w = color_rgba(q_N(N.x), q_N(N.y), q_N(N.z), u8(clampr(iFloor(w * 255.f + .5f), 0, 255)));
		_T = color_rgba(q_N(T.x), q_N(T.y), q_N(T.z), 0);
		_B = color_rgba(q_N(B.x), q_N(B.y), q_N(B.z), 0);

		_tc_i[0] = tc.x;
		_tc_i[1] = tc.y;
		_tc_i[2] = s16(index0);
		_tc_i[3] = s16(index1);
	}

	float get_weight() const
	{
		return float(color_get_A(_N_w)) / 255.f;
	}

	u16 get_bone(u16 w) const
	{
		return (u16)_tc_i[w + 2] / 3;
	}

	void get_pos(Fvector& p) const
	{
		p.x = _P[0];
		p.y = _P[1];
		p.z = _P[2];
	}

	/*void get_pos_bones(Fvector& p, XRayKinematicsLegacy* Parent) const
	{
		Fvector P0, P1;

		Fmatrix& xform0 = Parent->LL_GetBoneInstance(get_bone(0)).mRenderTransform;
		Fmatrix& xform1 = Parent->LL_GetBoneInstance(get_bone(1)).mRenderTransform;

		get_pos(P0);
		xform0.transform_tiny(P0);
		get_pos(P1);
		xform1.transform_tiny(P1);

		p.lerp(P0, P1, get_weight());
	}*/
};


struct vertHW_3W
{
	float _P[4];
	u32 _N_w;
	u32 _T_w;
	u32 _B_i;
	float _tc_i[4];

	void set(Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, Fvector2& tc, int index0, int index1, int index2, float w0, float w1)
	{
		N.normalize_safe();
		T.normalize_safe();
		B.normalize_safe();

		_P[0] = P.x;
		_P[1] = P.y;
		_P[2] = P.z;
		_P[3] = 1.f;

		_N_w = color_rgba(q_N(N.x), q_N(N.y), q_N(N.z), u8(clampr(iFloor(w0 * 255.f + .5f), 0, 255)));
		_T_w = color_rgba(q_N(T.x), q_N(T.y), q_N(T.z), u8(clampr(iFloor(w1 * 255.f + .5f), 0, 255)));
		_B_i = color_rgba(q_N(B.x), q_N(B.y), q_N(B.z), u8(index2));

		_tc_i[0] = tc.x;
		_tc_i[1] = tc.y;
		_tc_i[2] = s16(index0);
		_tc_i[3] = s16(index1);
	}

	float get_weight0() const
	{
		return float(color_get_A(_N_w)) / 255.f;
	}

	float get_weight1() const
	{
		return float(color_get_A(_T_w)) / 255.f;
	}

	u16 get_bone(u16 w) const
	{
		switch (w)
		{
		case 0:
		case 1:
			return (u16)_tc_i[w + 2] / 3;
		case 2:
			return (u16)color_get_A(_B_i) / 3;
		}

		R_ASSERT(0);

		return 0;
	}

	void get_pos(Fvector& p) const
	{
		p.x = _P[0];
		p.y = _P[1];
		p.z = _P[2];
	}

	//void get_pos_bones(Fvector& p, XRayKinematicsLegacy* Parent) const
	//{
	//	Fvector P0, P1, P2;

	//	Fmatrix& xform0 = Parent->LL_GetBoneInstance(get_bone(0)).mRenderTransform;
	//	Fmatrix& xform1 = Parent->LL_GetBoneInstance(get_bone(1)).mRenderTransform;
	//	Fmatrix& xform2 = Parent->LL_GetBoneInstance(get_bone(2)).mRenderTransform;

	//	get_pos(P0);
	//	xform0.transform_tiny(P0);
	//	get_pos(P1);
	//	xform1.transform_tiny(P1);
	//	get_pos(P2);
	//	xform2.transform_tiny(P2);

	//	float w0 = get_weight0();
	//	float w1 = get_weight1();

	//	P0.mul(w0);
	//	P1.mul(w1);
	//	P2.mul(1 - w0 - w1);

	//	p = P0;
	//	p.add(P1);
	//	p.add(P2);
	//}
};



struct vertHW_4W
{
	float _P[4];
	u32 _N_w;
	u32 _T_w;
	u32 _B_w;
	float _tc[2];
	u32 _i;

	void set(Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, Fvector2& tc, int index0, int index1, int index2, int index3, float w0, float w1, float w2)
	{
		N.normalize_safe();
		T.normalize_safe();
		B.normalize_safe();

		_P[0] = P.x;
		_P[1] = P.y;
		_P[2] = P.z;
		_P[3] = 1.f;

		_N_w = color_rgba(q_N(N.x), q_N(N.y), q_N(N.z), u8(clampr(iFloor(w0 * 255.f + .5f), 0, 255)));
		_T_w = color_rgba(q_N(T.x), q_N(T.y), q_N(T.z), u8(clampr(iFloor(w1 * 255.f + .5f), 0, 255)));
		_B_w = color_rgba(q_N(B.x), q_N(B.y), q_N(B.z), u8(clampr(iFloor(w2 * 255.f + .5f), 0, 255)));

		_tc[0] = tc.x;
		_tc[1] = tc.y;

		_i = color_rgba(u8(index0), u8(index1), u8(index2), u8(index3));
	}

	float get_weight0() const
	{
		return float(color_get_A(_N_w)) / 255.f;
	}

	float get_weight1() const
	{
		return float(color_get_A(_T_w)) / 255.f;
	}

	float get_weight2() const
	{
		return float(color_get_A(_B_w)) / 255.f;
	}

	u16 get_bone(u16 w) const
	{
		switch (w)
		{
		case 0:
			return (u16)color_get_R(_i) / 3;
		case 1:
			return (u16)color_get_G(_i) / 3;
		case 2:
			return (u16)color_get_B(_i) / 3;
		case 3:
			return (u16)color_get_A(_i) / 3;
		}

		R_ASSERT(0);

		return 0;
	}

	void get_pos(Fvector& p) const
	{
		p.x = _P[0];
		p.y = _P[1];
		p.z = _P[2];
	}

	//void get_pos_bones(Fvector& p, XRayKinematicsLegacy* Parent) const
	//{
	//	Fvector P[4];

	//	for (u16 i = 0; i < 4; ++i)
	//	{
	//		Fmatrix& xform = Parent->LL_GetBoneInstance(get_bone(i)).mRenderTransform;
	//		get_pos(P[i]);

	//		xform.transform_tiny(P[i]);
	//	}

	//	float w[3];

	//	w[0] = get_weight0();
	//	w[1] = get_weight1();
	//	w[2] = get_weight2();

	//	for (int j = 0; j < 3; ++j)
	//		P[j].mul(w[j]);

	//	P[3].mul(1 - w[0] - w[1] - w[2]);

	//	p = P[0];

	//	for (int k = 1; k < 4; ++k)
	//		p.add(P[k]);
	//}
};

#pragma pack(pop)


#define VLOAD_NOVERTICES 1<<0

XRaySkeletonMesh::XRaySkeletonMesh()
{
}

void XRaySkeletonMesh::Load(const char* N, IReader* data, u32 dwFlags)
{
	_Load(N, data, CountVertex);
	void* _verts_ = data->pointer();

	XRaySkeletonVisual::Load(N, data, dwFlags | VLOAD_NOVERTICES);
	{
		check(data->find_chunk(OGF_INDICES));
		CountIndex = data->r_u32();
	}
	_DuplicateIndices(N, data);

	_Load_hw(*this, _verts_);
}

void XRaySkeletonMesh::Copy(XRaySkeletonVisual* pFrom)
{
	XRaySkeletonVisual::Copy(pFrom);
	XRaySkeletonX::_Copy(pFrom->CastToXRaySkeletonX());
}

void XRaySkeletonXExt::_Load_hw(XRaySkeletonVisual& V, void* _verts_)
{
	switch (RenderMode)
	{
	case RM_SINGLE:
	case RM_SKINNING_1B:
	{
		u32 size = V.CountVertex * sizeof(vertBoned1W);
		u32 crc = crc32(_verts_, size);
		Vertices1W.create(crc, V.CountVertex, (vertBoned1W*)_verts_);
	}
	break;

	case RM_SKINNING_2B:
	{
		
		u32 size = V.CountVertex * sizeof(vertBoned2W);
		u32 crc = crc32(_verts_, size);
		Vertices2W.create(crc, V.CountVertex, (vertBoned2W*)_verts_);
	}
	break;

	case RM_SKINNING_3B:
	{
		
		u32 size = V.CountVertex * sizeof(vertBoned3W);
		u32 crc = crc32(_verts_, size);
		Vertices3W.create(crc, V.CountVertex, (vertBoned3W*)_verts_);
	}
	break;

	case RM_SKINNING_4B:
	{
		
		u32 size = V.CountVertex * sizeof(vertBoned4W);
		u32 crc = crc32(_verts_, size);
		Vertices4W.create(crc, V.CountVertex, (vertBoned4W*)_verts_);

	}
	break;
	}
}

//-----------------------------------------------------------------------------------------------------
// Wallmarks
//-----------------------------------------------------------------------------------------------------


void XRaySkeletonXExt::_CollectBoneFaces(XRaySkeletonVisual* V, size_t OffsetIndex, size_t CountIndex)
{
	u16* indices = 0;

	indices = *m_Indices;

	indices += OffsetIndex;

		if (*Vertices1W)
		{
			vertBoned1W* vertices = *Vertices1W;

			for (u32 idx = 0; idx < CountIndex; idx++)
			{
				vertBoned1W& v = vertices[V->OffsetVertex + indices[idx]];
				CBoneData& BD = ParentKinematics->LL_GetData((u16)v.matrix);

				BD.AppendFace(ChildIDX, (u16)(idx / 3));
			}
		}
		else if (*Vertices2W)
		{
			vertBoned2W* vertices = *Vertices2W;

			for (u32 idx = 0; idx < CountIndex; ++idx)
			{
				vertBoned2W& v = vertices[V->OffsetVertex + indices[idx]];
				CBoneData& BD0 = ParentKinematics->LL_GetData((u16)v.matrix0);
				BD0.AppendFace(ChildIDX, (u16)(idx / 3));
				CBoneData& BD1 = ParentKinematics->LL_GetData((u16)v.matrix1);
				BD1.AppendFace(ChildIDX, (u16)(idx / 3));
			}
		}
		else if (*Vertices3W)
		{
			vertBoned3W* vertices = *Vertices3W;

			for (u32 idx = 0; idx < CountIndex; ++idx)
			{
				vertBoned3W& v = vertices[V->OffsetVertex + indices[idx]];
				CBoneData& BD0 = ParentKinematics->LL_GetData((u16)v.m[0]);
				BD0.AppendFace(ChildIDX, (u16)(idx / 3));
				CBoneData& BD1 = ParentKinematics->LL_GetData((u16)v.m[1]);
				BD1.AppendFace(ChildIDX, (u16)(idx / 3));
				CBoneData& BD2 = ParentKinematics->LL_GetData((u16)v.m[2]);
				BD2.AppendFace(ChildIDX, (u16)(idx / 3));
			}
		}
		else if (*Vertices4W)
		{
			vertBoned4W* vertices = *Vertices4W;

			for (u32 idx = 0; idx < CountIndex; ++idx)
			{
				vertBoned4W& v = vertices[V->OffsetVertex + indices[idx]];
				CBoneData& BD0 = ParentKinematics->LL_GetData((u16)v.m[0]);
				BD0.AppendFace(ChildIDX, (u16)(idx / 3));
				CBoneData& BD1 = ParentKinematics->LL_GetData((u16)v.m[1]);
				BD1.AppendFace(ChildIDX, (u16)(idx / 3));

				CBoneData& BD2 = ParentKinematics->LL_GetData((u16)v.m[2]);
				BD2.AppendFace(ChildIDX, (u16)(idx / 3));
				CBoneData& BD3 = ParentKinematics->LL_GetData((u16)v.m[3]);
				BD3.AppendFace(ChildIDX, (u16)(idx / 3));
			}
		}
		else
			R_ASSERT2(0, "not implemented yet");
	
}

void XRaySkeletonMesh::AfterLoad(XRayKinematicsLegacy* parent, u16 child_idx)
{
	inherited2::AfterLoad(parent, child_idx);
	inherited2::_CollectBoneFaces(this, OffsetIndex, CountIndex);
}

//
//template <typename T>
//IC void get_pos_bones(const T& v, Fvector& p, XRayKinematicsLegacy* Parent)
//{
//	v.get_pos_bones(p, Parent);
//}

BOOL XRaySkeletonXExt::_PickBone(IKinematics::pick_result& r, float dist, const Fvector& start, const Fvector& dir, XRaySkeletonVisual* V, u16 bone_id, size_t OffsetIndex, size_t CountIndex)
{
	VERIFY(ParentKinematics && (ChildIDX != u16(-1)));

	CBoneData& BD = ParentKinematics->LL_GetData(bone_id);
	CBoneData::FacesVec* faces = &BD.child_faces[ChildIDX];

	BOOL result = FALSE;
	u16* indices = 0;

	indices = *m_Indices;

	if (*Vertices1W)
		result = _PickBoneSoft1W(r, dist, start, dir, indices + OffsetIndex, *faces);
	else if (*Vertices2W)
		result = _PickBoneSoft2W(r, dist, start, dir, indices + OffsetIndex, *faces);
	else if (*Vertices3W)
		result = _PickBoneSoft3W(r, dist, start, dir, indices + OffsetIndex, *faces);
	else
	{
		VERIFY(!!(*Vertices4W));

		result = _PickBoneSoft4W(r, dist, start, dir, indices + OffsetIndex, *faces);
	}

	return result;
}


BOOL XRaySkeletonMesh::PickBone(IKinematics::pick_result& r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id)
{
	return inherited2::_PickBone(r, dist, start, dir, this, bone_id, OffsetIndex, CountIndex);
}


void XRaySkeletonMesh::EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id)
{
	inherited2::_EnumBoneVertices(C, this, bone_id, OffsetIndex, CountIndex);
}


template <typename vertex_buffer_type>
IC void TEnumBoneVertices(vertex_buffer_type vertices, u16* indices, CBoneData::FacesVec& faces, SEnumVerticesCallback& C)
{
	for (auto it = faces.begin(); it != faces.end(); it++)
	{
		u32 idx = (*it) * 3;

		for (u32 k = 0; k < 3; k++)
		{
			Fvector P;
			vertices[indices[idx + k]].get_pos(P);

			C(P);
		}
	}
}

void XRaySkeletonXExt::_EnumBoneVertices(SEnumVerticesCallback& C, XRaySkeletonVisual* V, u16 bone_id, size_t OffsetIndex, size_t CountIndex) const
{

	VERIFY(ParentKinematics && (ChildIDX != u16(-1)));

	CBoneData& BD = ParentKinematics->LL_GetData(bone_id);
	CBoneData::FacesVec* faces = &BD.child_faces[ChildIDX];

	u16* indices = 0;

	VERIFY(*m_Indices);

	indices = *m_Indices;

	if (*Vertices1W)
		TEnumBoneVertices(Vertices1W, indices + OffsetIndex, *faces, C);
	else if (*Vertices2W)
		TEnumBoneVertices(Vertices2W, indices + OffsetIndex, *faces, C);
	else if (*Vertices3W)
		TEnumBoneVertices(Vertices3W, indices + OffsetIndex, *faces, C);
	else
	{
		VERIFY(!!(*Vertices4W));

		TEnumBoneVertices(Vertices4W, indices + OffsetIndex, *faces, C);
	}
}
