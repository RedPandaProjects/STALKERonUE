#pragma once
#include "StalkerKinematicsBoneData.h"
#include "StalkerKinematicsAnimsData.h"
#include "StalkerKinematicsAssetUserData.generated.h"


UCLASS()
class STALKER_API UStalkerKinematicsAssetUserData : public UAssetUserData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (MultiLine = true))
	FString	UserData;
	UPROPERTY(EditAnywhere)
	TMap<FName,FStalkerKinematicsBone> Bones;
	UPROPERTY(EditAnywhere)
	TArray<UStalkerKinematicsAnimsData*> Anims;


	void BuildFromLegacy(const  TArray<TSharedPtr<CBoneData>>& LegacyBones);
	void BuildBones(TArray<StalkerKinematicsBone>& LegacyBones);
};
