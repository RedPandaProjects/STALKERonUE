#include "XRayUIShader.h"

#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"


XRayUIShader::XRayUIShader()
{
	ShaderType = SHADER_TYPE_INVALID;
	Brush = nullptr;
	Material = nullptr;
}

XRayUIShader::~XRayUIShader()
{
	destroy();
}

void XRayUIShader::Copy(IUIShader& _in)
{
	destroy();

	XRayUIShader &other = static_cast<XRayUIShader&>(_in);

	ShaderType = other.ShaderType;
	MaterialName = other.MaterialName;
	TextureName = other.TextureName; 
	if (other.Material)
	{
		Material = GStalkerEngineManager->GetResourcesManager()->Copy(other.Material);
	}
	if (other.Brush)
	{
		Brush = GStalkerEngineManager->GetResourcesManager()->Copy(other.Brush);
	}
}

void XRayUIShader::create(LPCSTR sh, LPCSTR tex)
{
	FString strMaterialName(sh);
	MaterialName = sh;
	check(MaterialName != NAME_None);
	TextureName = tex? FName(tex) : NAME_None;
	if (MaterialName != NAME_None) {
		Material = GStalkerEngineManager->GetResourcesManager()->GetShaderMaterial(MaterialName, TextureName);
		if (Material) {
			if (strMaterialName.StartsWith("effects\\")) {
				// effect material
				ShaderType = SHADER_TYPE_MATERIAL;
			} else {
				// UI material
				ShaderType = SHADER_TYPE_UI;
				Brush = GStalkerEngineManager->GetResourcesManager()->GetBrush(Material);
			}
		}
	}
}

bool XRayUIShader::inited()
{
	return Material != nullptr;
}

void XRayUIShader::destroy()
{
	if (Brush)
	{
		GStalkerEngineManager->GetResourcesManager()->Free(Brush);
		Brush = nullptr;
	}
	if (Material)
	{
		GStalkerEngineManager->GetResourcesManager()->Free(Material);
		Material = nullptr;
	}
	ShaderType = SHADER_TYPE_INVALID;
}