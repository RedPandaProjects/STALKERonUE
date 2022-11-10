#pragma once
#include "XrRender/Public/RainRender.h"
class XRayRainRender:public IRainRender
{
public:
	XRayRainRender();
	virtual void Copy(IRainRender &_in) ;

	virtual void Render(CEffect_Rain&owner) ;

	virtual const Fsphere& GetDropBounds() const;
};