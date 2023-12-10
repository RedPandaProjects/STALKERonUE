#include "RBMKShadersManager.h"

#include "RBMKShaderBase.h"
#include "RBMKShaderParticle.h"
#include "RBMKShaderTerrain.h"
#include "RBMKShaderWallmark.h"
#include "StalkerEditorManager.h"
#include "Importer/FRBMKEngineFactory.h"
#include "Kernel/StalkerEngineManager.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"

FRBMKShadersManager::FRBMKShadersManager(FRBMKEngineFactory* InOwner):Owner(InOwner)
{
	Load();
}

FRBMKShadersManager::~FRBMKShadersManager()
{
}

void FRBMKShadersManager::Load()
{
	string_path ShaderFilePath;
	FS.update_path(ShaderFilePath, "$game_data$", "shaders.xr");

	if(FPaths::FileExists(ShaderFilePath))
	{
		FArrayReader Ar;
		FFileHelper::LoadFileToArray(Ar,ANSI_TO_TCHAR(ShaderFilePath));
		FArrayReader MainAr = FStalkerFileSystem::ReadChunk(Ar,2);

		for(uint32 i = 0;FStalkerFileSystem::FindChunk(MainAr,i);i++)
		{
			FArrayReader ShaderChunk = FStalkerFileSystem::ReadChunk(MainAr,i);
			FRBMKShaderDescription ShaderDescription;
			ShaderChunk<<ShaderDescription;
			ShaderChunk.Seek(0);
			TUniquePtr<FRBMKShaderBase> Shader = CreateShader(ShaderDescription.ClassID);
			Shader->Serialize(ShaderChunk);
			Shaders.Add(ShaderDescription.Name,MoveTemp(Shader));
		}
	}

	AddShader(MakeUnique<FRBMKShaderWallmark>(TEXT("effects\\wallmarkblend"),this));
	AddShader(MakeUnique<FRBMKShaderWallmark>(TEXT("effects\\wallmarkmult"),this));
	AddShader(MakeUnique<FRBMKShaderWallmark>(TEXT("effects\\wallmarkset"),this));
	AddShader(MakeUnique<FRBMKShaderParticle>(TEXT("particles\\xadd"),this));
	AddShader(MakeUnique<FRBMKShaderParticle>(TEXT("particles\\xdistort"),this));
}

UMaterialInterface* FRBMKShadersManager::ImportSurface(const FString& Path, const FString&ShaderName, const FString& TextureName, const FString& GameMaterial,bool HudMode)
{
	const UStalkerGameSettings* StalkerGameSettings = GetDefault<UStalkerGameSettings>();
	FString NewShaderName = ShaderName;
	if(const FString* InShaderName = StalkerGameSettings->ReplaceShaderWhenImport.Find(ShaderName))
	{
		NewShaderName = *InShaderName;
	}
	FRBMKShaderBase* Shader = GetOrCreateShader(NewShaderName);
	return Shader->ImportSurface(Path,TextureName,GameMaterial,HudMode);
}

UMaterialInterface* FRBMKShadersManager::GetOrCreateMasterMaterial(const FString& Name, const TSoftObjectPtr<UMaterialInterface>& UnknownMaterial) const
{
	const FString ParentName = Name.Replace(TEXT("\\"), TEXT("/"));

	UMaterialInterface* ParentMaterial;
	{
		const FString ParentPackageName = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Materials") / ParentName);
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	if (!IsValid(ParentMaterial))
	{
		const FString ParentPackageName = UPackageTools::SanitizePackageName(TEXT("/Game/Base/Materials") / ParentName);
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	if (!IsValid(ParentMaterial))
	{
		UMaterialInterface* InUnknownMaterial = UnknownMaterial.LoadSynchronous();
		if(ensure(IsValid(InUnknownMaterial)))
		{
			const FString ParentPackageName = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Materials") / ParentName);
			UPackage* AssetPackage = CreatePackage(*ParentPackageName);
			UMaterialInstanceConstant* NewParentMaterial = NewObject<UMaterialInstanceConstant>(AssetPackage, *FPaths::GetBaseFilename(ParentPackageName), Owner->GetObjectFlags());
			NewParentMaterial->Parent = InUnknownMaterial;
			FAssetRegistryModule::AssetCreated(NewParentMaterial);
			NewParentMaterial->Modify();
			NewParentMaterial->PostEditChange();
			ParentMaterial = NewParentMaterial;
		}
		else
		{
			return nullptr;
		}
	}
	return ParentMaterial;
}

FRBMKShaderTextureInfo FRBMKShadersManager::GetOrImportTexture(const FString& TextureName)
{
	if (GStalkerEngineManager->GetCurrentGame() == EStalkerGame::SHOC)
	{
		return GetOrImportTextureLegacy(TextureName);
	}

	FRBMKShaderTextureInfo Result;
	Result.BaseTexture = Owner->ImportTexture(TextureName);

	ETextureThumbnail MainTHM(TCHAR_TO_ANSI(*TextureName));
	if (MainTHM.Load(TCHAR_TO_ANSI(*TextureName)))
	{
		Result.Material = (static_cast<float>(MainTHM._Format().material) + MainTHM._Format().material_weight + .5f) / 4.f;
		if (MainTHM._Format().bump_mode == STextureParams::tbmUse || MainTHM._Format().bump_mode == STextureParams::tbmUseParallax)
		{
			Result.UseBump = true;
			TObjectPtr<UTexture2D> NormalMap;
			TObjectPtr<UTexture2D> HeightGloss;
			Owner->ImportBump2D(MainTHM._Format().bump_name.c_str(), NormalMap, HeightGloss);
			Result.NormalMapTexture = NormalMap;
			Result.HeightGlossTexture = HeightGloss;
			if(MainTHM._Format().bump_mode == STextureParams::tbmUseParallax)
			{
				Result.UseParallax = true;
				Result.ParallaxHeight = MainTHM._Format().bump_virtual_height / 5.f;
			}
		}
		if(MainTHM._Format().detail_name.size())
		{
			Result.DetailTexture = Owner->ImportTexture(ANSI_TO_TCHAR(MainTHM._Format().detail_name.c_str()));
			Result.DetailScale = MainTHM._Format().detail_scale;
			Result.UseDetail = true;
			ETextureThumbnail THMDetail(MainTHM._Format().detail_name.c_str());
			if (THMDetail.Load(MainTHM._Format().detail_name.c_str()))
			{
				if (THMDetail._Format().bump_mode == STextureParams::tbmUse )
				{
					TObjectPtr<UTexture2D> NormalMapTextureDetail;
					TObjectPtr<UTexture2D> HeightGlossTextureDetail;
					Owner->ImportBump2D(ANSI_TO_TCHAR(THMDetail._Format().bump_name.c_str()), NormalMapTextureDetail, HeightGlossTextureDetail);
					Result.DetailNormalMapTexture = NormalMapTextureDetail;
					Result.DetailHeightGlossTexture = HeightGlossTextureDetail;
				}
			}
		}
	}
	else
	{
		UE_LOG(LogXRayImporter, Warning, TEXT("Can't found thm %s"), *TextureName);
	}
	return Result;

}

FRBMKShaderTextureInfo FRBMKShadersManager::GetOrImportTextureLegacy(const FString& TextureName)
{
	LegacyTexturesInfoInitialize();
	FRBMKShaderTextureInfo Result;
	Result.BaseTexture = Owner->ImportTexture(TextureName);
	if (LegacyTexture2Bumps.Contains(TextureName))
	{
		const TPair<FString, float>& Bump = LegacyTexture2Bumps[TextureName];
		Result.Material = (Bump.Value + .5f) / 4.f;
		if(!Bump.Key.IsEmpty())
		{
			Result.UseBump = true;
			TObjectPtr<UTexture2D> NormalMap;
			TObjectPtr<UTexture2D> HeightGloss;
			Owner->ImportBump2D(Bump.Key, NormalMap, HeightGloss);
			Result.NormalMapTexture = NormalMap;
			Result.HeightGlossTexture = HeightGloss;
			Result.UseParallax = true;
			Result.ParallaxHeight = 0.05f / 5.f;
		}
	}
	if (LegacyTexture2Details.Contains(TextureName))
	{
		const TPair<FString, float>& Detail = LegacyTexture2Details[TextureName];
		Result.UseDetail = true;
		Result.DetailTexture = Owner->ImportTexture(Detail.Key);
		Result.DetailScale = Detail.Value;
		if (LegacyTexture2Bumps.Contains(Detail.Key))
		{
			const TPair<FString, float>& DetailBump = LegacyTexture2Bumps[Detail.Key];
			if(!DetailBump.Key.IsEmpty())
			{
				Result.UseDetailBump = true;
				TObjectPtr<UTexture2D> NormalMapTextureDetail;
				TObjectPtr<UTexture2D>  HeightGlossTextureDetail;
				Owner->ImportBump2D(DetailBump.Key, NormalMapTextureDetail, HeightGlossTextureDetail);
				Result.DetailNormalMapTexture = NormalMapTextureDetail;
				Result.DetailHeightGlossTexture = HeightGlossTextureDetail;
			}
		}
	}
	return Result;
}

void FRBMKShadersManager::LegacyTexturesInfoInitialize()
{
	if(LegacyHasInitialized)
	{
		return;
	}
	LegacyHasInitialized = true;
	string_path INIFileName;
	if (FS.exist(INIFileName, "$game_textures$", "textures.ltx"))
	{
		CInifile			ini(INIFileName);
		if (ini.section_exist("association"))
		{
			CInifile::Sect& data = ini.r_section("association");
			CInifile::SectCIt I = data.Data.begin();
			CInifile::SectCIt E = data.Data.end();
			for (; I != E; ++I)
			{
				const CInifile::Item& item = *I;

				string_path				TextureName;
				float					DetailScale;

				const int32 Result = sscanf_s(*item.second, "%[^,],%f", TextureName, static_cast<uint32>(sizeof(string_path)), &DetailScale);
				check(Result == 2);
				LegacyTexture2Details.Add(ANSI_TO_TCHAR(item.first.c_str()), TPair<FString, float>(ANSI_TO_TCHAR(TextureName), DetailScale));
			}
		}

		if (ini.section_exist("specification"))
		{
			CInifile::Sect& sect = ini.r_section("specification");
			for (CInifile::SectCIt I2 = sect.Data.begin(); I2 != sect.Data.end(); ++I2)
			{
				const CInifile::Item& item = *I2;


				string_path				BumpMode;
				float					Material;
				const int32 Result = sscanf_s(item.second.c_str(), "bump_mode[%[^]]], material[%f]", BumpMode,static_cast<uint32>(sizeof(string_path)), &Material);
				check(Result == 2);
				if ((BumpMode[0] == 'u') && (BumpMode[1] == 's') && (BumpMode[2] == 'e') && (BumpMode[3] == ':'))
				{
					LegacyTexture2Bumps.Add(ANSI_TO_TCHAR(item.first.c_str()), TPair<FString, float>(ANSI_TO_TCHAR(BumpMode+4), Material));
				}
				else
				{
					LegacyTexture2Bumps.Add(ANSI_TO_TCHAR(item.first.c_str()), TPair<FString, float>(TEXT(""),Material));
				}
			}
		}
	}
}

FRBMKShaderBase* FRBMKShadersManager::GetOrCreateShader(const FString& ShaderName)
{
	if(const TUniquePtr<FRBMKShaderBase>* Shader = Shaders.Find(ShaderName))
	{
		return Shader->Get();
	}
	TUniquePtr<FRBMKShaderBase> Shader = CreateShader();
	Shader->Description.Name = ShaderName;
	return Shaders.Add(ShaderName,MoveTemp(Shader)).Get();
}

TUniquePtr<FRBMKShaderBase> FRBMKShadersManager::CreateShader(uint64 ClassID)
{
	switch (ClassID)
	{
	case FStalkerTools::MakeClassID('B','m','m','D','o','l','d',' '):
		return MakeUnique<FRBMKShaderTerrain>(this);
	case FStalkerTools::MakeClassID('P','A','R','T','I','C','L','E'):
		return MakeUnique<FRBMKShaderParticle>(this);
	default:
		return MakeUnique<FRBMKShaderBase>(this);
	}
}

void FRBMKShadersManager::AddShader(TUniquePtr<FRBMKShaderBase> Shader)
{
	Shaders.Add(Shader->Description.Name,MoveTemp(Shader));
}
