#pragma once
#include "StalkerSpawnProperties_ALifeInventoryItem.generated.h"

UCLASS(DefaultToInstanced)
class  UStalkerSpawnProperties_ALifeInventoryItem : public UObject
{
	GENERATED_BODY()
public:
	void SetEntity		(ISE_Abstract* InEntity);
	void FillProperties	();

	UPROPERTY(EditAnywhere, Category = "Base")
	float	ItemCondition;
	UPROPERTY(EditAnywhere, Category = "ALife", meta = (DisplayName = "Useful for AI"))
	bool UsefulForAI;
	UPROPERTY(EditAnywhere, Category = "ALife", meta = (DisplayName = "Visible for AI"))
	bool VisibleForAI;
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	ISE_Abstract* Entity = nullptr;
	ISE_ALifeInventoryItem* ALifeInventoryItem = nullptr;
};