#pragma once
#include "StalkerEngineManager.generated.h"
UENUM(BlueprintType)
enum class EStalkerGame:uint8
{
	COP UMETA(DisplayName = "Call of Pripyat"),
	CS UMETA(DisplayName = "Clear Sky"),
	SHOC UMETA(DisplayName = "Shadow of Chernobyl"),
};
UCLASS()
class UStalkerEngineManager : public UObject
{
	GENERATED_BODY()
public:
	void											AttachViewport			(class UGameViewportClient* InGameViewportClient);
	void											DetachViewport			(class UGameViewportClient* InGameViewportClient);
	FName											GetCurrentLevel			();
	void											Initialized				();
	void											ReInitialized			(EStalkerGame Game);
	void											Destroy					();
	inline class UStalkerResourcesManager*			GetResourcesManager		()								{return ResourcesManager;	}
	inline class UWorld*							GetGameWorld			()								{return GameWorld; }
	void											SetInput				(class XRayInput* InXRayInput);
	inline class XRayInput*							GetInput				()								{return MyXRayInput;}
	inline class UStalkerGameViewportClient*		GetGameViewportClient	()								{return GameViewportClient;}

	FSimpleMulticastDelegate						ReInitializedMulticastDelegate;
	inline	EStalkerGame							GetCurrentGame			() const { return CurrentGame; }
private:
	void											OnViewportCloseRequested (FViewport* InViewport);
	void											OnViewportResized(FViewport* InViewport, uint32);
	void											OnEndPlayMap	();

	UPROPERTY(Transient)
	TObjectPtr<class UStalkerResourcesManager>		ResourcesManager;
	UPROPERTY(Transient)
	TObjectPtr < class UStalkerGameViewportClient>	GameViewportClient;
	UPROPERTY(Transient)
	TObjectPtr < class UWorld>						GameWorld;

	class XRayEngine*								MyXRayEngine;
	class XRayMemory*								GXRayMemory;
	class XRayLog*									GXRayLog;
	class XRayDebug*								GXRayDebug;
	class XRayInput*								MyXRayInput;
	FDelegateHandle									DelegateHandleOnViewportResized;
	FDelegateHandle									DelegateHandleOnViewportCloseRequested;
	EStalkerGame									CurrentGame;
	FDelegateHandle									DelegateHandleOnEndPlayMap;
};

extern STALKER_API UStalkerEngineManager* GXRayEngineManager;