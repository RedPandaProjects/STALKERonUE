#pragma once
#include "NiagaraSystem.h"
#include "StalkerNiagaraSystem.generated.h"
USTRUCT()
struct FStalkerEmiterMaterial
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UMaterialInterface*Material;
	
	UPROPERTY(EditAnywhere)
	UMaterialInterface*HudMaterial;

};

UCLASS()
class STALKER_API UStalkerNiagaraSystem : public UNiagaraSystem
{
	GENERATED_BODY()

public:
	virtual void PreSave(FObjectPreSaveContext TargetPlatform) override;

	/** Metadata for xrgame **/
	UPROPERTY(EditAnywhere,Category = "Stalker")
	bool IsLooping = false;
	
	/** Metadata for xrgame **/
	UPROPERTY(EditAnywhere,Category = "Stalker")
	TMap<FName,FStalkerEmiterMaterial> Materials;
};
