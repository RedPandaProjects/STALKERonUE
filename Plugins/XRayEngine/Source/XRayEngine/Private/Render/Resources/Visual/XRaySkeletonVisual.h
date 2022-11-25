#pragma once
class XRaySkeletonVisual : public IRenderVisual
{
	
public:
											XRaySkeletonVisual();
	virtual									~XRaySkeletonVisual();
	virtual void							Load(const char* N, IReader* data, u32 dwFlags);
	virtual void							Copy(XRaySkeletonVisual* Right);
	virtual void							Spawn();
	virtual void							Depart();
	virtual bool							Release();
	vis_data&								getVisData() override;
	u32										getType() override;
	shared_str								getDebugName() override;

	xr_vector< XRaySkeletonVisual*>			Children;
	shared_str								Name;
	u32										Type;				// visual's type
	vis_data								VisData;				// visibility-data
	int32									RefCount;
	XRaySkeletonVisual*						Parent;
	virtual	class	XRaySkeletonX*			CastToXRaySkeletonX() { return nullptr; }
	virtual	class XRayKinematics*			CastToXRayKinematics(){return nullptr;}
	virtual XRaySkeletonVisual*				CastToRaySkeletonVisual() { return this; }

	size_t	CountVertex;
	size_t	CountIndex;
	size_t	OffsetIndex;
	size_t	OffsetVertex;

};