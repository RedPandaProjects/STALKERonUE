#pragma once
#include "RBMKShaderBase.h"

class FRBMKShaderWallmark: public FRBMKShaderBase
{
public:
						FRBMKShaderWallmark	(const FString& Name, FRBMKShadersManager* Owner);
	UMaterialInterface* ImportSurface		(const FString& Path, const FString& TextureName,const FString& GameMaterial, bool HudMode) override;

};
