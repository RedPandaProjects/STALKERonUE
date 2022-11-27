#include "XRayEngine.h"
#include "XRayDevice.h"
#include "../Render/Interface/XRayRenderInterface.h"
#include "../Render/Interface/XRayRenderFactory.h"
#include "../Render/Interface/XRayDUInterface.h"
#include "../Render/Interface/XRayDebugRender.h"
#include "../Render/Interface/UI/XRayUIRender.h"
#include "XRayInput.h"
#include "../Base/EngineManager/XRayEngineManager.h"
XRayRenderFactory GRenderFactory;
XRayDUInterface  GDUInterface;
XRayDebugRender GDebugRender;
XRayEngine::XRayEngine()
{

}

void XRayEngine::Initialize()
{
	Device = new XRayDevice;
	XRayInput* MyInput = new XRayInput;
	GXRayEngineManager->SetInput(MyInput);
	GXRayInput = MyInput;
	::Render = &GRenderInterface;
	::RenderFactory = &GRenderFactory;
	::DU = &GDUInterface;
	UIRender = &GXRayUIRender;
	DRender = &GDebugRender;
	XRayEngineInterface::Initialize();
}

void XRayEngine::Destroy()
{
	GXRayEngineManager->SetInput(nullptr);
	XRayEngineInterface::Destroy();
	delete GXRayInput;
	delete Device;
}
