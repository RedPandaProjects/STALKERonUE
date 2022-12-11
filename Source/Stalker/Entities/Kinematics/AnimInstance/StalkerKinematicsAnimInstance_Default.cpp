#include "StalkerKinematicsAnimInstance_Default.h"
#include "StalkerKinematicsAnimInstanceProxy.h"

struct FStalkerKinematicsAnimInstanceProxy& UStalkerKinematicsAnimInstance_Default::GetProxy()
{
	return GetProxyOnGameThread< FStalkerKinematicsAnimInstanceProxy> ();
}

bool UStalkerKinematicsAnimInstance_Default::CanRunParallelWork() const
{
	return true;
}

FAnimInstanceProxy* UStalkerKinematicsAnimInstance_Default::CreateAnimInstanceProxy()
{
	return new FStalkerKinematicsAnimInstanceProxy(this);
}
