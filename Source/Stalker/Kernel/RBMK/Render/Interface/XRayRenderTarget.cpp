#include "XRayRenderTarget.h"
XRayRenderTarget GRenderTarget;
XRayRenderTarget::XRayRenderTarget()
{
	Blur = 0;
	Gray = 0;
	Duality.Set(0,0);
	Noise = 0;
	NoiseScale = 1;
	NoiseFPS = 1;
}
XRayRenderTarget::~XRayRenderTarget() 
{

}
void XRayRenderTarget::set_blur(float f)
{
	Blur = f;
}

void XRayRenderTarget::set_gray(float f)
{
	Gray = f;
}

void XRayRenderTarget::set_duality_h(float f)
{
	Duality.X = FMath::Abs(f);
}

void XRayRenderTarget::set_duality_v(float f)
{
	Duality.Y = FMath::Abs(f);
}

void XRayRenderTarget::set_noise(float f)
{
	Noise = f;
}

void XRayRenderTarget::set_noise_scale(float f)
{
	NoiseScale = f;
}

void XRayRenderTarget::set_noise_fps(float f)
{
	NoiseFPS = FMath::Abs(f) + EPS_S;
}

void XRayRenderTarget::set_color_base(u32 f)
{
	FColor InColor(f);
	//Swap(InColor.R,InColor.B);
	BaseColor = InColor.ReinterpretAsLinear();
}

void XRayRenderTarget::set_color_gray(u32 f)
{
	FColor InColor(f);
	//Swap(InColor.R,InColor.B);
	GrayColor = InColor.ReinterpretAsLinear();
}

void XRayRenderTarget::set_color_add(const Fvector& f)
{
	AddColor.R = f.x;
	AddColor.G = f.y;
	AddColor.B = f.z;
	AddColor.A = 0.f;
}

u32 XRayRenderTarget::get_width()
{
	return Device->dwWidth;
}

u32 XRayRenderTarget::get_height()
{
	return Device->dwHeight;
}

void XRayRenderTarget::set_cm_imfluence(float f)
{
	Grad = f;
}

void XRayRenderTarget::set_cm_interpolate(float f)
{
	GradInterpolate = f;
}

void XRayRenderTarget::set_cm_textures(const shared_str& tex0, const shared_str& tex1)
{
	GradTexture0 = tex0;
	GradTexture1 = tex1;
}
