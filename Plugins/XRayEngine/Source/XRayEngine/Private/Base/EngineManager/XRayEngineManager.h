#pragma once
#include "XRayEngineManager.generated.h"

UCLASS()
class UXRayEngineManager : public UObject
{
	GENERATED_BODY()
public:
	void										AttachViewport			(class UGameViewportClient* InGameViewportClient);
	void										DetachViewport			(class UGameViewportClient* InGameViewportClient);
	void										Initialized				();
	void										Destroy					();
	inline class UXRayResourcesManager*			GetResourcesManager		()								{return ResourcesManager;	}
	inline class UWorld*						GetGameWorld			()								{return GameWorld; }
	void										SetInput				(class XRayInput* InXRayInput);
	inline class XRayInput*						GetInput				()								{return MyXRayInput;}
	inline class UXRayGameViewportClient*		GetGameViewportClient	()								{return GameViewportClient;}
private:
	void										OnViewportCloseRequested (FViewport* InViewport);
	void										OnViewportResized(FViewport* InViewport, uint32);

	UPROPERTY(Transient)
	TObjectPtr<class UXRayResourcesManager>		ResourcesManager;
	UPROPERTY(Transient)
	TObjectPtr < class UXRayGameViewportClient> GameViewportClient;
	UPROPERTY(Transient)
	TObjectPtr < class UWorld>					GameWorld;
	
	class XRayMemory*							GXRayMemory;
	class XRayLog*								GXRayLog;
	class XRayDebug*							GXRayDebug;
	class XRayInput*							MyXRayInput;
	FDelegateHandle								DelegateHandleOnViewportResized;
	FDelegateHandle								DelegateHandleOnViewportCloseRequested;
};

extern UXRayEngineManager* GXRayEngineManager;