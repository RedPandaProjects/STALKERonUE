#pragma once
#include "StalkerSpawnProperties_Base.h"
#include "StalkerSpawnPropertiesTypes.h"
#include "StalkerSpawnProperties_ALifeGraphPoint.generated.h"

UCLASS()
class  UStalkerSpawnProperties_ALifeGraphPoint : public UStalkerSpawnProperties_Base
{
	GENERATED_BODY()
public:
	void				SetEntity(ISE_Abstract* InEntity) override;
	void				FillProperties() override;
	void				PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;


protected:
	ISE_ALifeGraphPoint* ALifeGraphPoint = nullptr;

	UPROPERTY(EditAnywhere, Category = "Location")
	FStalkerSpawnData_Location1 Location1;

	UPROPERTY(EditAnywhere, Category = "Location")
	FStalkerSpawnData_Location2 Location2;

	UPROPERTY(EditAnywhere, Category = "Location")
	FStalkerSpawnData_Location3 Location3;

	UPROPERTY(EditAnywhere, Category = "Location")
	FStalkerSpawnData_Location4 Location4;

	UPROPERTY( EditAnywhere,Category = "Connection", meta = (AllowedClasses = "/Script/Engine.World"))
	FSoftObjectPath Map;

	UPROPERTY(EditAnywhere, Category = "Connection")
	FString PointName;
private:
	void		SetMap();
};