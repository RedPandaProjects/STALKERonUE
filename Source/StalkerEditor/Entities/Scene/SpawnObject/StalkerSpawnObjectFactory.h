#pragma once
#include "ActorFactories/ActorFactory.h"
#include "StalkerSpawnObjectFactory.generated.h"

UCLASS()
class  UStalkerSpawnObjectFactory : public UActorFactory
{
	GENERATED_BODY()
public:
	UStalkerSpawnObjectFactory();
private:
	bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;

protected:
	AActor* SpawnActor(UObject* InAsset, ULevel* InLevel, const FTransform& InTransform, const FActorSpawnParameters& InSpawnParams) override;


	FString GetDefaultActorLabel(UObject* Asset) const override;

};
