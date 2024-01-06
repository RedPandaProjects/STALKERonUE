#pragma once

class XRayRenderTarget :public IRender_Target
{
public:
					XRayRenderTarget	();
					~XRayRenderTarget	() override;
	 void			set_blur			(float f) override;
	 void			set_gray			(float f) override;
	 void			set_duality_h		(float f) override;
	 void			set_duality_v		(float f) override;
	 void			set_noise			(float f) override;
	 void			set_noise_scale		(float f) override;
	 void			set_noise_fps		(float f) override;
	 void			set_color_base		(u32 f) override;
	 void			set_color_gray		(u32 f) override;
	 void			set_color_add		(const Fvector& f)override;
	 u32			get_width			()override;
	 u32			get_height			()override;
	 void			set_cm_imfluence	(float f)override;
	 void			set_cm_interpolate	(float f)override;
	 void			set_cm_textures		(const shared_str& tex0, const shared_str& tex1)override;

	float			Blur;
	float			Gray;
	FVector2f		Duality;
	float			Noise;
	float			NoiseScale;
	float			NoiseFPS;
	FLinearColor	BaseColor;
	FLinearColor	GrayColor;
	FLinearColor	AddColor;

	float			Grad;
	float			GradInterpolate;
	shared_str		GradTexture0;
	shared_str		GradTexture1;
};
extern XRayRenderTarget GRenderTarget;