#include "XRayRenderFactory.h"
#include "Core/XRayApplicationRender.h"
#include "Core/XRayRenderDeviceRender.h"
#include "Debug/XRayObjectSpaceRender.h"
#include "Debug/XRayStatGraphRender.h"
#include "Debug/XRayStatsRender.h"
#include "Environment/XRayEnvironmentRender.h"
#include "Environment/XRayFlareRender.h"
#include "Environment/XRayLensFlareRender.h"
#include "Environment/XRayRainRender.h"
#include "Thunderbolt/XRayThunderboltDescRender.h"
#include "Thunderbolt/XRayThunderboltRender.h"
#include "UI/XRayConsoleRender.h"
#include "UI/XRayFontRender.h"
#include "UI/XRayUIRender.h"
#include "UI/XRayUISequenceVideoItem.h"
#include "WallMark/XRayWallMarkArray.h"
#include "XRayDebugRender.h"
#include "XRayDUInterface.h"
#include "XRayRenderInterface.h"
#undef RENDER_FACTORY_INTERFACE
#define RENDER_FACTORY_INTERFACE(Class)\
I ## Class* XRayRenderFactory::Create ## Class()\
{\
	return static_cast<I ## Class*>(new XRay ## Class);\
}\
void XRayRenderFactory::Destroy##Class(I ## Class *pObject)\
{\
	delete pObject;\
}

#ifndef _EDITOR
RENDER_FACTORY_INTERFACE(UISequenceVideoItem)
RENDER_FACTORY_INTERFACE(UIShader)
RENDER_FACTORY_INTERFACE(StatGraphRender)
RENDER_FACTORY_INTERFACE(ConsoleRender)
RENDER_FACTORY_INTERFACE(RenderDeviceRender)
RENDER_FACTORY_INTERFACE(ObjectSpaceRender)
RENDER_FACTORY_INTERFACE(WallMarkArray)
RENDER_FACTORY_INTERFACE(StatsRender)
#endif // _EDITOR

#ifndef _EDITOR
RENDER_FACTORY_INTERFACE(EnvironmentRender)
RENDER_FACTORY_INTERFACE(EnvDescriptorMixerRender)
RENDER_FACTORY_INTERFACE(EnvDescriptorRender)
RENDER_FACTORY_INTERFACE(RainRender)
RENDER_FACTORY_INTERFACE(LensFlareRender)
RENDER_FACTORY_INTERFACE(ThunderboltRender)
RENDER_FACTORY_INTERFACE(ThunderboltDescRender)
RENDER_FACTORY_INTERFACE(FlareRender)
#endif // _EDITOR
RENDER_FACTORY_INTERFACE(FontRender)

RENDER_FACTORY_INTERFACE(ApplicationRender)