#include "StalkerKinematicsAnimInstance_Default.h"
#include "StalkerKinematicsAnimInstanceProxy.h"

FAnimInstanceProxy* UStalkerKinematicsAnimInstance_Default::CreateAnimInstanceProxy()
{
	return new FStalkerKinematicsAnimInstanceProxy(this);
}
