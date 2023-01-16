#pragma once
#include "StalkerWorldSettings.generated.h"

UCLASS()
class STALKER_API AStalkerWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	class UStalkerCForm*		GetOrCreateCForm	();
	class UStalkerCForm*		GetCForm			();
	class UStalkerAIMap*		GetOrCreateAIMap	();
	class UStalkerAIMap*		GetAIMap			();

	void						PostInitProperties	() override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString						AlifeName;
	UPROPERTY(Transient)
	class AStalkerDebugRender*	DebugRender;
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
#endif
private:
	UPROPERTY()
	class UStalkerCForm* CForm;
	UPROPERTY()
	class UStalkerAIMap* AIMap;
};
