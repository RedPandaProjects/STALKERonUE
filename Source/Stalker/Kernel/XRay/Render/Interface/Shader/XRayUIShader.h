#pragma once

#include "Resources/StalkerResourcesManager.h"
#include "XrRender/Public/UIShader.h"

// despite its name, it's used by X-Ray Engine
// not just for UI, but also for decals, particles etc.

class XRayUIShader :public IUIShader
{
public:
	enum Type {
		SHADER_TYPE_INVALID,
		SHADER_TYPE_UI,
		SHADER_TYPE_MATERIAL
	} ShaderType;

	XRayUIShader();
	~XRayUIShader() override;
	void Copy(IUIShader& _in) override;
	void create(LPCSTR sh, LPCSTR tex = 0) override;
	bool inited() override;
	void destroy() override;
	FName MaterialName;
	FName TextureName;
	FStalkerResourcesManager::ShaderMaterialInfo *Material;
	TObjectPtr<USlateBrushAsset> Brush;
};