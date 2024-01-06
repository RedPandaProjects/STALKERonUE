#include "XRayRenderDeviceRender.h"

XRayRenderDeviceRender::XRayRenderDeviceRender()
{

}

XRayRenderDeviceRender::~XRayRenderDeviceRender()
{

}

void XRayRenderDeviceRender::Copy(IRenderDeviceRender & _in)
{
	R_ASSERT(0);
}

float XRayRenderDeviceRender::GetGamma() const
{
	return 0.0f;
}

float XRayRenderDeviceRender::GetBrightness() const
{
	return 0.0f;
}

float XRayRenderDeviceRender::GetContrast() const
{
	return 0.0f;
}

Fvector XRayRenderDeviceRender::GetBalance() const
{
	return Fvector();
}

void XRayRenderDeviceRender::setGamma(float fGamma)
{
}

void XRayRenderDeviceRender::setBrightness(float fGamma)
{
}

void XRayRenderDeviceRender::setContrast(float fGamma)
{
}

void XRayRenderDeviceRender::setBalance(float r, float g, float b)
{
}

void XRayRenderDeviceRender::setBalance(Fvector& C)
{
}

void XRayRenderDeviceRender::updateGamma()
{
}




void XRayRenderDeviceRender::OnDeviceDestroy(BOOL bKeepTextures)
{

}

void XRayRenderDeviceRender::ValidateHW()
{
}

void XRayRenderDeviceRender::DestroyHW()
{

	
}

void XRayRenderDeviceRender::Reset(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2)
{

}

void XRayRenderDeviceRender::SetupStates()
{
}

void XRayRenderDeviceRender::OnDeviceCreate(LPCSTR shName)
{


}

void XRayRenderDeviceRender::Create(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2, bool)
{
}




void XRayRenderDeviceRender::SetupGPU(BOOL bForceGPU_SW, BOOL bForceGPU_NonPure, BOOL bForceGPU_REF)
{
}

void XRayRenderDeviceRender::overdrawBegin()
{
}

void XRayRenderDeviceRender::overdrawEnd()
{
}

void XRayRenderDeviceRender::DeferredLoad(BOOL E)
{
}

void XRayRenderDeviceRender::ResourcesDeferredUpload()
{
}

void XRayRenderDeviceRender::ResourcesDestroyNecessaryTextures()
{
}

void XRayRenderDeviceRender::ResourcesStoreNecessaryTextures()
{
}


bool XRayRenderDeviceRender::HWSupportsShaderYUV2RGB()
{
	return true;
}

u32 XRayRenderDeviceRender::GetGPULoadPercent()
{
	return u32();
}

IRenderDeviceRender::DeviceState XRayRenderDeviceRender::GetDeviceState()
{
	return DeviceState();
}

BOOL XRayRenderDeviceRender::GetForceGPU_REF()
{
	return 0;
}

u32 XRayRenderDeviceRender::GetCacheStatPolys()
{
	return 0;
}

void XRayRenderDeviceRender::Begin()
{
	
}

void XRayRenderDeviceRender::Clear()
{
	
}

void XRayRenderDeviceRender::End()
{
	
}

void XRayRenderDeviceRender::ClearTarget()
{
}

void XRayRenderDeviceRender::SetCacheXform( Fmatrix& mView,  Fmatrix& mProject)
{
}


void XRayRenderDeviceRender::OnAssetsChanged()
{
}

void XRayRenderDeviceRender::ResourcesDumpMemoryUsage()
{
}

void XRayRenderDeviceRender::ResourcesGetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps)
{
}
