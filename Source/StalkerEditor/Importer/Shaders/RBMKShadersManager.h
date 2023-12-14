#pragma once
#include "RBMKShaderBase.h"

struct FRBMKShaderTextureInfo
{
	TSoftObjectPtr<UTexture2D>	BaseTexture;

	float						Material = 0;

	bool						UseBump = false;
	bool						UseParallax = false;
	float						ParallaxHeight = 0.01f;
	TSoftObjectPtr<UTexture2D>	NormalMapTexture;
	TSoftObjectPtr<UTexture2D>	HeightGlossTexture;

	bool						UseDetail = false;
	float						DetailScale = 1;
	TSoftObjectPtr<UTexture2D>	DetailTexture;
	bool						UseDetailBump = false;
	TSoftObjectPtr<UTexture2D>	DetailNormalMapTexture;
	TSoftObjectPtr<UTexture2D>	DetailHeightGlossTexture;

};

class FRBMKEngineFactory;
class FRBMKShadersManager
{
public:
												FRBMKShadersManager				(FRBMKEngineFactory* Owner);
												~FRBMKShadersManager			();
	UMaterialInterface*							ImportSurface					(const FString& Path, const FString&ShaderName, const FString& TextureName, const FString& GameMaterial,bool HudMode = false);
	UMaterialInterface*							GetOrCreateMasterMaterial		(const FString&Name,const TSoftObjectPtr<UMaterialInterface>&UnknownMaterial) const;
	FRBMKShaderTextureInfo						GetOrImportTexture				(const FString& TextureName);
	void										ImportTextures					(const TArray<FString>&Prefixes,const TArray<FString>& IgnorePrefixes);
	FRBMKEngineFactory*							Owner;
private:
	void										Load							();
	FRBMKShaderTextureInfo						GetOrImportTextureLegacy		(const FString& TextureName);
	void										LegacyTexturesInfoInitialize	();
	FRBMKShaderBase*							GetOrCreateShader				(const FString&ShaderName);
	TUniquePtr<FRBMKShaderBase>					CreateShader					(uint64 ClassID = -1);
	void										AddShader						(TUniquePtr<FRBMKShaderBase> Shader);	
	TMap<FString,TUniquePtr<FRBMKShaderBase>>	Shaders;
	bool										LegacyHasInitialized			= false;
	TMap<FString, TPair<FString, float>>		LegacyTexture2Details;
	TMap<FString, TPair<FString, float>>		LegacyTexture2Bumps;
};
