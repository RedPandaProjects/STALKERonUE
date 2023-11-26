#pragma once
#include "StalkerStaticMeshAssetUserData.generated.h"

UCLASS()
class STALKER_API UStalkerStaticMeshAssetUserData : public UAssetUserData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	bool IsOnlyCollision = false;
};
