#pragma once
#include "StalkerKinematicsAnimAssetUserData.h"
#include "StalkerKinematicsAnimsData.generated.h"
USTRUCT()
struct STALKER_API FStalkerKinematicsAnimsBonesPart
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSet<FName> Bones;
};

UCLASS()
class STALKER_API UStalkerKinematicsAnimsData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TMap<FName, UAnimSequence*>  Anims;

	UPROPERTY(EditAnywhere)
	TMap<FName, FStalkerKinematicsAnimsBonesPart> BonesParts;

};
