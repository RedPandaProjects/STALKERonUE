#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrCore/stdafx.h"
THIRD_PARTY_INCLUDES_END
class FRBMKDebug :public XRayDebugInterface
{
public:
	FRBMKDebug();
	void CriticalError(const char* Text) override;
};