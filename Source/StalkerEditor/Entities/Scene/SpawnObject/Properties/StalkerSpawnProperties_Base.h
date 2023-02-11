#pragma once
#include "StalkerSpawnProperties_Base.generated.h"

UCLASS(DefaultToInstanced)
class  UStalkerSpawnProperties_Base : public UObject
{
	GENERATED_BODY()
public:
	virtual void SetEntity		(ISE_Abstract* InEntity);
	virtual void FillProperties	();
protected:
	ISE_Abstract*Entity = nullptr;
};