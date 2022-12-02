#pragma once
class XRaySkeletonMeshManager
{
public:
	XRaySkeletonMeshManager();
	~XRaySkeletonMeshManager();
	class XRaySkeletonVisual* Get(shared_str Name, IReader* InFS);
	class XRaySkeletonVisual* Get(shared_str Name);
	class XRaySkeletonVisual* Dublicate(class XRaySkeletonVisual*  In);
	void  Destroy(class XRaySkeletonVisual* In);
	void Flush();
private:
	class XRaySkeletonVisual* Load(shared_str Name);
	class XRaySkeletonVisual* Create(u32 Type);
	TMap<shared_str,class XRaySkeletonVisual*> Meshes;
	TArray<class XRaySkeletonVisual*>			Pool;
	bool									bIsDiscardDestroy;
};
extern XRaySkeletonMeshManager*GXRaySkeletonMeshManager;