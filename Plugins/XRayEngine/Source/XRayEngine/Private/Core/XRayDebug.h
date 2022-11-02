#pragma once
#include "XrCore/stdafx.h"
class XRayDebug :public XRayDebugInterface
{
public:
	XRayDebug();
	void CriticalError(const char* Text) override;
};