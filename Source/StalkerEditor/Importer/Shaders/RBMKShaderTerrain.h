#pragma once
#include "RBMKShaderBase.h"

class FRBMKShaderTerrain: public FRBMKShaderBase
{
public:
	FRBMKShaderTerrain(FRBMKShadersManager* Owner);

	~FRBMKShaderTerrain() override;
	void Serialize(FArchive& Ar) override;
	UMaterialInterface* ImportSurface(const FString& Path, const FString& TextureName,const FString& GameMaterial, bool HudMode) override;

protected:
	FString DetailTextureName;
	FString RTextureName;
	FString GTextureName;
	FString BTextureName;
	FString ATextureName;
};
