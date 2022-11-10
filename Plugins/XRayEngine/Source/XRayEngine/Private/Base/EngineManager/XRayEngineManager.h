#pragma once
#include "XRayEngineManager.generated.h"

UCLASS()
class UXRayEngineManager : public UObject
{
	GENERATED_BODY()
public:
	void AttachViewport(class UGameViewportClient* InGameViewportClient);
	void DetachViewport(class UGameViewportClient* InGameViewportClient);
	void Initialized();
	void Destroy();
	inline class UXRayResourcesManager*GetResourcesManager(){return ResourcesManager;	}
private:
	void	OnViewportResized(FViewport*, uint32);
	UPROPERTY(Transient)
	TObjectPtr<class UXRayResourcesManager> ResourcesManager;
	UPROPERTY(Transient)
	TObjectPtr < class UGameViewportClient> GameViewportClient;

	class XRayMemory* GXRayMemory;
	class XRayLog* GXRayLog;
	class XRayDebug* GXRayDebug;
	FDelegateHandle DelegateHandleOnViewportResized;
};

extern UXRayEngineManager* GXRayEngineManager;