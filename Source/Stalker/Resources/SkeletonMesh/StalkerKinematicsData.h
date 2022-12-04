#pragma once
#include "StalkerKinematicsBoneData.h"
#include "StalkerKinematicsData.generated.h"


UCLASS()
class STALKER_API UStalkerKinematicsData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	class USkeletalMesh*	Mesh;
	UPROPERTY(EditAnywhere, meta = (MultiLine = true))
	FString	UserData;
	UPROPERTY(EditAnywhere)
	TMap<FName,FStalkerKinematicsBone> Bones;

	void BuildFromLegacy(const  TArray<TSharedPtr<CBoneData>>& LegacyBones);
	void BuildToLegacy(TArray<TSharedPtr<CBoneData>>& LegacyBones);
};
