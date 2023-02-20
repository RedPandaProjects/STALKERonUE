#include "StalkerSpawnObjectFactory.h"
#include "StalkerSpawnObject.h"

UStalkerSpawnObjectFactory::UStalkerSpawnObjectFactory()
{
	NewActorClass = AStalkerSpawnObject::StaticClass();
}

bool UStalkerSpawnObjectFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{	
	if (AssetData.GetClass() == NewActorClass)
	{
		return true;
	}
	return Super::CanCreateActorFrom(AssetData, OutErrorMsg);
}

AActor* UStalkerSpawnObjectFactory::SpawnActor(UObject* InAsset, ULevel* InLevel, const FTransform& InTransform, const FActorSpawnParameters& InSpawnParams)
{
	FActorSpawnParameters  SpawnParams = InSpawnParams;
	SpawnParams.Template = Cast<AActor>(InAsset);
	return Super::SpawnActor(InAsset, InLevel, InTransform, SpawnParams);
}

FString UStalkerSpawnObjectFactory::GetDefaultActorLabel(UObject* InAsset) const
{
	if (AActor* Actor = Cast<AActor>(InAsset))
	{
		return Actor->GetDefaultActorLabel();
	}
	return Super::GetDefaultActorLabel(InAsset);
}
