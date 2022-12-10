#include "XRayEngine.h"
#include "XRayDevice.h"
#include "../Render/Interface/XRayRenderInterface.h"
#include "../Render/Interface/XRayRenderFactory.h"
#include "../Render/Interface/XRayDUInterface.h"
#include "../Render/Interface/XRayDebugRender.h"
#include "../Render/Interface/UI/XRayUIRender.h"
#include "XRayInput.h"
#include "../../StalkerEngineManager.h"
#include "XRayConsole.h"
XRayRenderFactory GRenderFactory;
XRayDUInterface  GDUInterface;
XRayDebugRender GDebugRender;
XRayEngine::XRayEngine()
{

}

void XRayEngine::Initialize()
{
	Device = new XRayDevice;
	Console = new  XRayConsole;

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
	delete Console;
	delete Device;
}

void XRayEngine::Level_Scan()
{
	XRayEngineInterface::Level_Scan();
}

int XRayEngine::Level_ID(LPCSTR name, LPCSTR ver, bool bSet)
{
	return XRayEngineInterface::Level_ID(name,ver,bSet);
}

void XRayEngine::Level_Set(u32 ID)
{
	XRayEngineInterface::Level_Set(ID);
	string_path Name;
	FCStringAnsi::Strcpy(Name, Levels[ID].folder);
	if (FCStringAnsi::Strlen(Name) && Name[FCStringAnsi::Strlen(Name) - 1] == '\\')
	{
		Name[FCStringAnsi::Strlen(Name) - 1]=0;
	}
	CurrentLevelName = Name;
	
}

void XRayEngine::LoadAllArchives()
{
}
