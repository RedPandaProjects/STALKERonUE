#pragma once
#include "StalkerWorldSettings.generated.h"

UCLASS()
class STALKER_API AStalkerWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	class UStalkerCForm*		GetCForm			();
	class UStalkerAIMap*		GetAIMap			();
#if WITH_EDITORONLY_DATA
	class UStalkerCForm*		GetOrCreateCForm	();
	class UStalkerAIMap*		GetOrCreateAIMap	();
	class UStalkerLevelSpawn*	GetOrCreateSpawn	();
	class UStalkerLevelSpawn*	GetSpawn			();
#endif

	void						PostInitProperties	() override;
	UPROPERTY(Transient)
	class AStalkerDebugRender*	DebugRender;
	
	UPROPERTY(Transient)
	class AStalkerEnvironment*	Environment;
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UMaterialInterface*			EditorMaterialAIMap;
	UPROPERTY(EditAnywhere,Category="Stalker|Editor")
	float						DistanceRenderAIMap = 35;
	UPROPERTY(EditAnywhere, Category = "Stalker|Editor")
	float						AIMapCanUP = 150.f;
	UPROPERTY(EditAnywhere, Category = "Stalker|Editor")
	float						AIMapCanDOWN = 400.f;
	UPROPERTY(EditAnywhere, Category = "Stalker|Editor")
	float						AIMapTestHeight = 150.f;
	UPROPERTY()
	bool						NeedRebuildSpawn = true;
#endif
	UPROPERTY(EditAnywhere)
	bool						NotForXRay		 = false;
private:

	UPROPERTY()
	class UStalkerCForm* CForm;
	UPROPERTY()
	class UStalkerAIMap* AIMap;
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UStalkerLevelSpawn* Spawn;
#endif
};
