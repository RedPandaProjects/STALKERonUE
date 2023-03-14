#pragma once
#include "StalkerEngineManager.generated.h"
UENUM(BlueprintType)
enum class EStalkerGame:uint8
{
	COP UMETA(DisplayName = "Call of Pripyat"),
	CS UMETA(DisplayName = "Clear Sky"),
	SHOC UMETA(DisplayName = "Shadow of Chernobyl"),
};
enum class EStalkerWorldStatus :uint8
{
	None,
	Ready,
	Loading,
	Failure
};
class STALKER_API FStalkerEngineManager : public FGCObject
{
public:
														FStalkerEngineManager				();
														~FStalkerEngineManager				();
	void												Initialize							();
	void												AddReferencedObjects				(FReferenceCollector& Collector) override;
	FString												GetReferencerName					() const override;
	void												AttachViewport						(class UGameViewportClient* InGameViewportClient);
	void												DetachViewport						(class UGameViewportClient* InGameViewportClient);

	void												AppStart							();
	void												AppEnd								();

	void												ReInitialized						(EStalkerGame Game);
	inline class FStalkerResourcesManager*				GetResourcesManager					()								{return ResourcesManager;	}

	void												SetInput							(class XRayInput* InXRayInput);
	inline class XRayInput*								GetInput							()								{return MyXRayInput;}

	inline class UStalkerGameViewportClient*			GetGameViewportClient				()								{return GameViewportClient;}

	void												LoadDefaultWorld					();
	bool												LoadWorld							(FString LevelName);
	inline EStalkerWorldStatus							GetWorldStatus						() const{return WorldStatus;}
	inline	EStalkerGame								GetCurrentGame						() const { return CurrentGame; }
	inline class UStalkerPhysicalMaterialsManager*		GetPhysicalMaterialsManager			() { return PhysicalMaterialsManager.Get(); }
	bool												CheckCurrentWorld					();
	FString												GetCurrentWorldName					();
	class UStalkerAIMap*								GetLevelGraph						(FString LevelName);
	//TArray< CShapeData::shape_def> DebugShapes;

	FSimpleMulticastDelegate							PreReInitializedMulticastDelegate;
	FSimpleMulticastDelegate							PostReInitializedMulticastDelegate;


private:

	void												OnPostEngineInit					();
	void												OnPostLoadMap						(UWorld* World);
	void												OnTravelFailure						(UWorld*NextWorld, ETravelFailure::Type FailureType, const FString& Name);

	void												OnViewportCloseRequested			(FViewport* InViewport);
	void												OnViewportResized					(FViewport* InViewport, uint32);
	void												OnEndPlayMap						();
#if WITH_EDITORONLY_DATA
	void												OnGetOnScreenMessages				(FCoreDelegates::FSeverityMessageMap& Out);
#endif
	FStalkerResourcesManager*							ResourcesManager = nullptr;
	TObjectPtr<class  UStalkerGameViewportClient>		GameViewportClient;
	TObjectPtr<class  UStalkerPhysicalMaterialsManager>	PhysicalMaterialsManager;
	TObjectPtr<class  UStalkerAIMap>					CurrentAIMap;
	class XRayEngine*									MyXRayEngine = nullptr;
	class XRayInput*									MyXRayInput = nullptr;
	EStalkerGame										CurrentGame;

	EStalkerWorldStatus									WorldStatus = EStalkerWorldStatus::None;
	FSoftObjectPath										CurrentWorldPath;
	FString												CurrentWorldName;

};

extern STALKER_API FStalkerEngineManager* GStalkerEngineManager;