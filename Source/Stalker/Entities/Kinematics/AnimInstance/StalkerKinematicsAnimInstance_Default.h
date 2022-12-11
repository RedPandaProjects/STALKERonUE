#pragma once
#include "Animation/AnimInstance.h"
#include "StalkerKinematicsAnimInstance_Default.generated.h"

UCLASS()
class STALKER_API UStalkerKinematicsAnimInstance_Default : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	struct FStalkerKinematicsAnimInstanceProxy& GetProxy();

	bool CanRunParallelWork() const override;

protected:
	FAnimInstanceProxy* CreateAnimInstanceProxy() override;

};
