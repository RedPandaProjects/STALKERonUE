#pragma once
#include "Animation/AnimInstanceProxy.h"
#include "../AnimNode/StalkerAnimNode_Kinematics.h"
#include "StalkerKinematicsAnimInstanceProxy.generated.h"

USTRUCT()
struct STALKER_API FStalkerKinematicsAnimInstanceProxy:public FAnimInstanceProxy
{
	GENERATED_USTRUCT_BODY()
public:
	FStalkerKinematicsAnimInstanceProxy();
	FStalkerKinematicsAnimInstanceProxy(UAnimInstance* Instance);
	FStalkerAnimNode_Kinematics& GetKinematicsRootNode() {return KinematicsRootNode;}
protected:
	FAnimNode_Base* GetCustomRootNode() override;
	UPROPERTY()
	FStalkerAnimNode_Kinematics KinematicsRootNode;
};