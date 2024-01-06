#pragma once
#include "XrEngine/Render/ConsoleRender.h"
class XRayConsoleRender :public IConsoleRender
{
public:
	XRayConsoleRender();
	virtual void Copy(IConsoleRender &_in);
	virtual void OnRender(bool bGame);
private:
};