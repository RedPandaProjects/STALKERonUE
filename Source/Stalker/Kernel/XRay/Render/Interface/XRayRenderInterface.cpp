#include "XRayRenderInterface.h"
#include "XRayRenderTarget.h"
#include "../Resources/SkeletonMesh/XRaySkeletonMeshManager.h"
#include "../Resources/Visual/XRaySkeletonVisual.h"
#include "../../../StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"
#include "Entities/Kinematics/StalkerKinematics.h"
#include "Entities/Levels/Light/StalkerLight.h"
XRayRenderInterface GRenderInterface;

XRayRenderInterface::XRayRenderInterface()
{
	InMatrix = nullptr;
	InVisible = false;
}

bool XRayRenderInterface::is_sun_static()
{
	return false;
}

DWORD XRayRenderInterface::get_dx_level()
{
	return 0;
}

void XRayRenderInterface::create()
{
}

void XRayRenderInterface::destroy()
{
}

void XRayRenderInterface::reset_begin()
{
}

void XRayRenderInterface::reset_end()
{
}

void XRayRenderInterface::level_Load(IReader*)
{
}

void XRayRenderInterface::level_Unload()
{
}

HRESULT XRayRenderInterface::shader_compile(LPCSTR name, DWORD const* pSrcData, UINT SrcDataLen, LPCSTR pFunctionName, LPCSTR pTarget, DWORD Flags, void*& result)
{
	return E_NOTIMPL;
}

LPCSTR XRayRenderInterface::getShaderPath()
{
	return LPCSTR();
}

IRender_Sector* XRayRenderInterface::getSector(int id)
{
	return nullptr;
}

IRenderVisual* XRayRenderInterface::getVisual(int id)
{
	return nullptr;
}

IRender_Sector* XRayRenderInterface::detectSector(const Fvector& P)
{
	return nullptr;
}

IRender_Target* XRayRenderInterface::getTarget()
{
	return &GRenderTarget;
}

void XRayRenderInterface::set_Transform(Fmatrix* M)
{
	InMatrix = M;
}

void XRayRenderInterface::set_HUD(BOOL V)
{
}

BOOL XRayRenderInterface::get_HUD()
{
	return 0;
}

void XRayRenderInterface::set_Invisible(BOOL V)
{
	InVisible = V;
}

void XRayRenderInterface::set_Object(IRenderable* O)
{
}

void XRayRenderInterface::add_Occluder(Fbox2& bb_screenspace)
{
}

void XRayRenderInterface::add_Visual(IRenderVisual* V)
{
	if (InVisible)
	{
		return;
	}
}

void XRayRenderInterface::add_Geometry(IRenderVisual* V)
{
}

void XRayRenderInterface::add_StaticWallmark(const wm_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V)
{
}

void XRayRenderInterface::add_StaticWallmark(IWallMarkArray* pArray, const Fvector& P, float s, CDB::TRI* T, Fvector* V)
{
}

void XRayRenderInterface::add_SkeletonWallmark(const Fmatrix* xf, IKinematics* obj, IWallMarkArray* pArray, const Fvector& start, const Fvector& dir, float size)
{
}

void XRayRenderInterface::clear_static_wallmarks()
{
}

void XRayRenderInterface::flush()
{
}
class XRayRenderObjectSpecific :public IRender_ObjectSpecific
{

public:
	XRayRenderObjectSpecific(){}
	~XRayRenderObjectSpecific() override {}
	void force_mode(u32 mode) override
	{
	}


	float get_luminocity() override
	{
		return 1;
	}


	float get_luminocity_hemi() override
	{
		return 1;
	}


	float* get_luminocity_hemi_cube() override
	{
		static float Result[6]={};
		return Result;
	}

};
IRender_ObjectSpecific* XRayRenderInterface::ros_create(IRenderable* parent)
{
	return new XRayRenderObjectSpecific;
}

void XRayRenderInterface::ros_destroy(IRender_ObjectSpecific*& V)
{
	if(V) 
	delete V;
	V= nullptr;
}
class XrayRenderLight :public IRender_Light
{
	public:

		void set_type(LT type) override
		{
		}

		bool IsActive = true;
		void set_active(bool InIsActive) override
		{
			IsActive = InIsActive;
		}


		bool get_active() override
		{
			return IsActive;
		}


		void set_shadow(bool) override
		{
		}


		void set_volumetric(bool) override
		{
		}


		void set_volumetric_quality(float) override
		{
		}


		void set_volumetric_intensity(float) override
		{
		}


		void set_volumetric_distance(float) override
		{
		}


		void set_position(const Fvector& P) override
		{
		}


		void set_rotation(const Fvector& D, const Fvector& R) override
		{
		}


		void set_cone(float angle) override
		{
		}


		void set_range(float R) override
		{
		}


		void set_virtual_size(float R) override
		{
		}


		void set_texture(LPCSTR name) override
		{
		}


		void set_color(const Fcolor& C) override
		{
		}


		void set_color(float r, float g, float b) override
		{
		}

		bool IsHUD = false;
		void set_hud_mode(bool b) override
		{
			IsHUD = b;
		}


		bool get_hud_mode() override
		{
			return IsHUD;;
		}

};
IRender_Light* XRayRenderInterface::light_create()
{
	return GXRayEngineManager->GetResourcesManager()->CreateLight();
}
class XRayRenderGlow :public IRender_Glow
{

public:
	bool IsActive = true;
	void set_active(bool InIsActive) override
	{
		IsActive = InIsActive;
	}


	bool get_active() override
	{
		return IsActive;
	}


	void set_position(const Fvector& P) override
	{
	}


	void set_direction(const Fvector& P) override
	{
	}


	void set_radius(float R) override
	{
	}


	void set_texture(LPCSTR name) override
	{
	}


	void set_color(const Fcolor& C) override
	{
	}


	void set_color(float r, float g, float b) override
	{
	}

};
IRender_Glow* XRayRenderInterface::glow_create()
{
	return new XRayRenderGlow;
}

IRenderVisual* XRayRenderInterface::model_CreateParticles(LPCSTR name)
{
	return nullptr;
}

IRenderVisual* XRayRenderInterface::model_Create(LPCSTR name, IReader* data)
{
	
	UStalkerKinematicsComponent* Kinematics =  GXRayEngineManager->GetResourcesManager()->CreateKinematics(name);
	if (Kinematics)
	{
		return Kinematics;
	}
	return GXRaySkeletonMeshManager->Get(name);
}

IRenderVisual* XRayRenderInterface::model_CreateChild(LPCSTR name, IReader* data)
{
	return nullptr;
}

IRenderVisual* XRayRenderInterface::model_Duplicate(IRenderVisual* V)
{
	return nullptr;
}

void XRayRenderInterface::model_Delete(IRenderVisual*& V, BOOL bDiscard)
{
	if (V)
	{	
		if (V->CastToStalkerKinematicsComponent())
		{
			GXRayEngineManager->GetResourcesManager()->Destroy(V->CastToStalkerKinematicsComponent());
		}
		else
		{
			check(false);
			//GXRaySkeletonMeshManager->Destroy(V->CastToRaySkeletonVisual());
		}
		V = nullptr;
	}
}

void XRayRenderInterface::model_Logging(BOOL bEnable)
{
}

void XRayRenderInterface::models_Prefetch()
{
}

void XRayRenderInterface::models_Clear(BOOL b_complete)
{
}

BOOL XRayRenderInterface::occ_visible(vis_data& V)
{
	return 0;
}

BOOL XRayRenderInterface::occ_visible(Fbox& B)
{
	return 0;
}

BOOL XRayRenderInterface::occ_visible(sPoly& P)
{
	return 0;
}

void XRayRenderInterface::Screenshot(ScreenshotMode mode, LPCSTR name)
{
}

void XRayRenderInterface::Screenshot(ScreenshotMode mode, CMemoryWriter& memory_writer)
{
}

void XRayRenderInterface::ScreenshotAsyncBegin()
{
}

void XRayRenderInterface::ScreenshotAsyncEnd(CMemoryWriter& memory_writer)
{
}

void XRayRenderInterface::rmNear()
{
}

void XRayRenderInterface::rmFar()
{
}

void XRayRenderInterface::rmNormal()
{
}

u32 XRayRenderInterface::memory_usage()
{
	return u32();
}

void XRayRenderInterface::BeforeWorldRender()
{
}

void XRayRenderInterface::AfterWorldRender()
{
}

void XRayRenderInterface::ChangeMark(LPCSTR mark)
{
}

u32 XRayRenderInterface::active_phase()
{
	return u32();
}

void XRayRenderInterface::Render()
{
}

void XRayRenderInterface::OnFrame()
{
}

void XRayRenderInterface::Calculate()
{
}

void XRayRenderInterface::ScreenshotImpl(ScreenshotMode mode, LPCSTR name, CMemoryWriter* memory_writer)
{
}

void XRayRenderInterface::light_destroy(IRender_Light* p_)
{
	if (p_)
	{
		GXRayEngineManager->GetResourcesManager()->Desotry(p_);
	}
	p_ = nullptr;
}

void XRayRenderInterface::glow_destroy(IRender_Glow* p_)
{
	if (p_)
	{
		delete p_;
	}
	p_ = nullptr;
}

