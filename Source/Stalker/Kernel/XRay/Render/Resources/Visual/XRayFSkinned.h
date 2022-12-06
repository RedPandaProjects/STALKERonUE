#pragma once
#include "XrRender/Public/RenderVisual.h"
#include "XrRender/Public/Kinematics.h"
#include "../SkeletonMesh/XRaySkeletonX.h"
#include "XRaySkeletonVisual.h"
struct SEnumVerticesCallback
{
	virtual void operator () (const Fvector& p) = 0;
};


class		XRaySkeletonXExt : public XRaySkeletonX	// shared code for SkeletonX derivates
{
protected:
	virtual void			_Load_hw(XRaySkeletonVisual& V, void* data);
	virtual void			_CollectBoneFaces(XRaySkeletonVisual* V, size_t iBase, size_t iCount);
	void			        _EnumBoneVertices(SEnumVerticesCallback& C, XRaySkeletonVisual* V, u16 bone_id, size_t iBase, size_t iCount) const;

	virtual BOOL			_PickBone(IKinematics::pick_result& r, float range, const Fvector& S, const Fvector& D, XRaySkeletonVisual* V, u16 bone_id, size_t iBase, size_t iCount);
public:
};

class XRaySkeletonMesh : public XRaySkeletonVisual, public XRaySkeletonXExt
{
private:
	typedef XRaySkeletonXExt	inherited2;
public:
	XRaySkeletonMesh();
	virtual					~XRaySkeletonMesh() {}
	virtual	XRaySkeletonX*	CastToXRaySkeletonX() { return this; }
	virtual void			Load(const char* N, IReader* data, u32 dwFlags);
	virtual void			Copy(XRaySkeletonVisual* pFrom);
	virtual void			AfterLoad(XRayKinematicsLegacy* parent, u16 child_idx);
	virtual void			EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id);
	virtual BOOL			PickBone(IKinematics::pick_result& r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id);

};
