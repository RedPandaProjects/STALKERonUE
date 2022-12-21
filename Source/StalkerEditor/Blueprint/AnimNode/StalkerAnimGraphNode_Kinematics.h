#pragma once
#include "Entities/Kinematics/AnimNode/StalkerAnimNode_Kinematics.h"
#include "AnimGraphNode_Base.h"
#include "StalkerAnimGraphNode_Kinematics.generated.h"
UCLASS(MinimalAPI)
class UStalkerAnimGraphNode_Kinematics : public UAnimGraphNode_Base
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = Settings)
	FStalkerAnimNode_Kinematics Node;
};