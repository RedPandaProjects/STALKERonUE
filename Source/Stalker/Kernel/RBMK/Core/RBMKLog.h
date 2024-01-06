#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrCore/stdafx.h"
THIRD_PARTY_INCLUDES_END
class FRBMKLog:public XRayLogInterface
{
public:
	FRBMKLog();
	void Log(const char* Text) override;
};