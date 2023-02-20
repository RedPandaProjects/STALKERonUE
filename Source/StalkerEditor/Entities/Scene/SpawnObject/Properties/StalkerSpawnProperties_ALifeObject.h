#pragma once
#include "StalkerSpawnProperties_Base.h"
#include "StalkerSpawnPropertiesTypes.h"
#include "StalkerSpawnProperties_ALifeObject.generated.h"

UCLASS()
class  UStalkerSpawnProperties_ALifeObject : public UStalkerSpawnProperties_Base
{
	GENERATED_BODY()
public:
	void				SetEntity				(ISE_Abstract* InEntity) override;
	void				FillProperties			() override;
	void				PostEditChangeProperty	(struct FPropertyChangedEvent& PropertyChangedEvent) override;


protected:
	ISE_ALifeObject*	ALifeObject = nullptr;

	UPROPERTY(EditAnywhere, Category = "Base", meta = (MultiLine = true))
	FString				CustomData;

	UPROPERTY(EditAnywhere, Category = "ALife")
	bool				CanSwitchOnline;
	UPROPERTY(EditAnywhere, Category = "ALife")
	bool				CanSwitchOffline;

	UPROPERTY(EditAnywhere, Category = "ALife")
	bool				Interactive;
	UPROPERTY(EditAnywhere, Category = "ALife")
	bool				UsedAILocations;

	UPROPERTY(EditAnywhere,Category = "ALife")
	FStalkerSpawnData_Storys Story;

	UPROPERTY(EditAnywhere,Category = "ALife")
	FStalkerSpawnData_SpawnStorys SpawnStory;
};