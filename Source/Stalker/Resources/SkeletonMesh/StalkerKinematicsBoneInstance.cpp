#include "StalkerKinematicsBoneInstance.h"

StalkerKinematicsBoneInstance::StalkerKinematicsBoneInstance()
{
	bUpdated = false;
}

const Fmatrix& StalkerKinematicsBoneInstance::GetTransform() const
{
	return Transform;
}

void StalkerKinematicsBoneInstance::SetTransform(const Fmatrix& val)
{
	Transform = val;
	bUpdated = true;
}
