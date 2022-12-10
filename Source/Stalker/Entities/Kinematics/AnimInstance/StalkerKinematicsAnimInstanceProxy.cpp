#include "StalkerKinematicsAnimInstanceProxy.h"
#include "Animation/AnimNodeBase.h"

FStalkerKinematicsAnimInstanceProxy::FStalkerKinematicsAnimInstanceProxy(UAnimInstance* Instance):FAnimInstanceProxy(Instance)
{

}

FStalkerKinematicsAnimInstanceProxy::FStalkerKinematicsAnimInstanceProxy()
{

}

FAnimNode_Base* FStalkerKinematicsAnimInstanceProxy::GetCustomRootNode()
{
	return &KinematicsRootNode;
}
