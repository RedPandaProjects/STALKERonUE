#pragma once
#ifdef RENDER_FACTORY_INTERFACE
#undef RENDER_FACTORY_INTERFACE
#endif
#include "XrEngine/Render/RenderFactory.h"
#define RENDER_FACTORY_INTERFACE(Class) \
virtual I ## Class* Create ## Class(); \
virtual void Destroy##Class(I ## Class *pObject);


class XRayRenderFactory:public IRenderFactory
{
public:
#ifndef _EDITOR
	/*
	virtual IStatsRender* CreateStatsRender() = 0;
	virtual void DestroyStatsRender(IStatsRender *pObject) = 0;
	*/
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
protected:
	//virtual IEnvDescriptorRender *CreateEnvDescriptorRender() = 0;
	//virtual void DestroyEnvDescriptorRender(IEnvDescriptorRender *pObject) = 0;
};
