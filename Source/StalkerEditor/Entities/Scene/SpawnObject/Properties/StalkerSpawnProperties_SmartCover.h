#pragma once
#include "StalkerSpawnProperties_ALifeObject.h"
#include "StalkerSpawnProperties_SmartCover.generated.h"


UCLASS()
class  UStalkerSpawnProperties_SmartCover : public UStalkerSpawnProperties_ALifeObject
{
	GENERATED_BODY()
public:
	void				SetEntity				(ISE_Abstract* InEntity) override;
	void				FillProperties			() override;
	void				PostEditChangeProperty	(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "Smart Cover")
	FStalkerSpawnData_SmartCoverDescription  Description;
	UPROPERTY(EditAnywhere, Category = "Smart Cover")
	float				HoldPositionTime;
	UPROPERTY(EditAnywhere, Category = "Smart Cover")
	float				ExitMinEnemyDistance;
	UPROPERTY(EditAnywhere, Category = "Smart Cover")
	float				EnterMinEnemyDistance;
	UPROPERTY(EditAnywhere, Category = "Smart Cover")
	bool				IsCombatCover;
	UPROPERTY(EditAnywhere, Category = "Smart Cover")
	bool				CanFire;
	UPROPERTY(EditAnywhere, EditFixedSize, NoClear, Category = "Smart Cover")
	TArray<FStalkerSpawnData_Loophole>	Loopholes;
protected:
	ISE_SmartCover*		SmartCover;
};
