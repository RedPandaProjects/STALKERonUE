#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XRayEngineInterface.h"
THIRD_PARTY_INCLUDES_END
class XRayEngine :public XRayEngineInterface
{
public:
	XRayEngine();
	void Initialize	() override;
	void Destroy	() override;

};