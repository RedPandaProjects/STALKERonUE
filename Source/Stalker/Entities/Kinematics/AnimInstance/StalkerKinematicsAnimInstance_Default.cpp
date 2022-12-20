#include "StalkerKinematicsAnimInstance_Default.h"
#include "StalkerKinematicsAnimInstanceProxy.h"

struct FStalkerKinematicsAnimInstanceProxy& UStalkerKinematicsAnimInstance_Default::GetProxy()
{
	return GetProxyOnGameThread< FStalkerKinematicsAnimInstanceProxy> ();
}

bool UStalkerKinematicsAnimInstance_Default::CanRunParallelWork() const
{
	return false;
}

FAnimInstanceProxy* UStalkerKinematicsAnimInstance_Default::CreateAnimInstanceProxy()
{
	return new FStalkerKinematicsAnimInstanceProxy(this);
}
