#pragma once
#include "StalkerPhysicalMaterialPairsData.generated.h"

USTRUCT(BlueprintType)
struct STALKER_API FStalkerPhysicalMaterialPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> BreakingSounds;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> StepSounds;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> CollideSounds;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> CollideParticles;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> CollideMarks;

	void BuildFromLegacy(const class SGameMtlPair& Legacy);
	void BuildToLegacy(class SGameMtlPair& Legacy);

};

USTRUCT(BlueprintType)
struct STALKER_API FStalkerPhysicalMaterialPairConteniar
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TMap<class UStalkerPhysicalMaterial*, FStalkerPhysicalMaterialPair>  Materials;

};

UCLASS(BlueprintType)
class STALKER_API UStalkerPhysicalMaterialPairsData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<class UStalkerPhysicalMaterial*, FStalkerPhysicalMaterialPairConteniar>  Pairs;
};
