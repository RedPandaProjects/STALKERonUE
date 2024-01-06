#pragma once
class XRayRenderInterface :public IRender_interface
{
public:
							XRayRenderInterface		();
	IRender_Target*			getTarget				() override;

	IRender_ObjectSpecific* ros_create				(IRenderable* parent) override;
	void					ros_destroy				(IRender_ObjectSpecific*&) override;

	IRender_Light*			light_create			() override;
	IRender_Glow*			glow_create				() override;

	IRenderVisual*			model_CreateParticles	(LPCSTR name) override;
	IRenderVisual*			model_Create			(LPCSTR name, IReader* data = 0) override;
	void					model_Delete			(IRenderVisual*& V, BOOL bDiscard = FALSE) override;
	void					light_destroy			(IRender_Light* p_) override;
	void					glow_destroy			(IRender_Glow* p_) override;
	void					SpawnStaticDecal		(const shared_str& TextureName, const Fvector& Point, const Fvector& Normal,float Size) override;
	void					SpawnSkeletalDecal		(const IRenderVisual* InObject, const shared_str& TextureName,const Fvector& StartPoint, const Fvector& EndPoint, float Size) override;
};
 extern XRayRenderInterface GRenderInterface;