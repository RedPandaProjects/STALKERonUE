#pragma once
#include "StalkerAnimNode_Kinematics.h"
#include "StalkerAnimGraphNode_Kinematics.generated.h"

UCLASS(MinimalAPI)
class UStalkerAnimGraphNode_Kinematics : public UAnimGraphNode_Base
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = Settings)
	FStalkerAnimNode_Kinematics Node;
};