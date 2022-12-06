#include "XRayBoneInstanceLegacy.h"

const Fmatrix& XRayBoneInstanceLegacy::GetTransform() const
{
	return mTransform;
}

void XRayBoneInstanceLegacy::SetTransform(const Fmatrix& val)
{
	mTransform = val;
}

void XRayBoneInstanceLegacy::construct()
{
	
	SetTransform(Fidentity);
	mRenderTransform.identity();
}
