#pragma once
#include "XrCore/stdafx.h"
class XRayLog:public XRayLogInterface
{
public:
	XRayLog();
	void Log(const char* Text) override;
};