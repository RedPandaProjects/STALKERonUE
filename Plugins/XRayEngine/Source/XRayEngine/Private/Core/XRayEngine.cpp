#include "XRayEngine.h"
#include "XRayDevice.h"
#include "../Render/Interface/XRayRenderInterface.h"
#include "../Render/Interface/XRayRenderFactory.h"
#include "../Render/Interface/XRayDUInterface.h"
#include "../Render/Interface/XRayDebugRender.h"
#include "../Render/Interface/UI/XRayUIRender.h"
XRayRenderFactory GRenderFactory;
XRayDUInterface  GDUInterface;
XRayDebugRender GDebugRender;
XRayEngine::XRayEngine()
{

}

void XRayEngine::Initialize()
{
	Device = new XRayDevice;
	::Render = &GRenderInterface;
	::RenderFactory = &GRenderFactory;
	::DU = &GDUInterface;
	UIRender = &GXRayUIRender;
	DRender = &GDebugRender;
	XRayEngineInterface::Initialize();
}

void XRayEngine::Destroy()
{
	XRayEngineInterface::Destroy();
	delete Device;
}
