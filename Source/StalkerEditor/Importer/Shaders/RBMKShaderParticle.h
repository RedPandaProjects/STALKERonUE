#pragma once
#include "RBMKShaderBase.h"

class FRBMKShaderParticle: public FRBMKShaderBase
{
public:
						FRBMKShaderParticle	(FRBMKShadersManager* Owner);
						FRBMKShaderParticle	(const FString&Name,FRBMKShadersManager* Owner);
	UMaterialInterface* ImportSurface		(const FString& Path, const FString& TextureName,const FString& GameMaterial, bool HudMode) override;

};
