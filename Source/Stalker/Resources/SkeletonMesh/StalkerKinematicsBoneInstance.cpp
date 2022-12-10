#include "StalkerKinematicsBoneInstance.h"

const Fmatrix& StalkerKinematicsBoneInstance::GetTransform() const
{
	return Transform;
}

void StalkerKinematicsBoneInstance::SetTransform(const Fmatrix& val)
{
	Transform = val;
}
