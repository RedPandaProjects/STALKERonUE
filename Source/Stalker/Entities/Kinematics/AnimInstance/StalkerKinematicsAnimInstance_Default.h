#pragma once
#include "StalkerKinematicsAnimInstance_Default.generated.h"

UCLASS()
class STALKER_API UStalkerKinematicsAnimInstance_Default : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	FAnimInstanceProxy* CreateAnimInstanceProxy() override;

};
