#pragma once
#include "XrEngine/XRayEngineInterface.h"
class XRayEngine :public XRayEngineInterface
{
public:
	XRayEngine();
	void Initialize	() override;
	void Destroy	() override;

};