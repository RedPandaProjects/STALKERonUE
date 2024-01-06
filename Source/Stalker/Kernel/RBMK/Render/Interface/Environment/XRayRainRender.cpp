#include "XRayRainRender.h"

XRayRainRender::XRayRainRender()
{
}

void XRayRainRender::Copy(IRainRender & _in)
{
}

void XRayRainRender::Render(CEffect_Rain& owner)
{
}

const Fsphere & XRayRainRender::GetDropBounds() const
{
	static Fsphere Result;
	return Result;
}
