#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrCore/stdafx.h"
THIRD_PARTY_INCLUDES_END
class XRayLog:public XRayLogInterface
{
public:
	XRayLog();
	void Log(const char* Text) override;
};