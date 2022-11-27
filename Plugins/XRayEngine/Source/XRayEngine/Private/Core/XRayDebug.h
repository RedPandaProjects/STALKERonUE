#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrCore/stdafx.h"
THIRD_PARTY_INCLUDES_END
class XRayDebug :public XRayDebugInterface
{
public:
	XRayDebug();
	void CriticalError(const char* Text) override;
};