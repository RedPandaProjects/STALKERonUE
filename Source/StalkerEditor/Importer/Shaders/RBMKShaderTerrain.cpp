#include "RBMKShaderTerrain.h"

#include "StalkerEditorManager.h"
#include "Importer/FRBMKEngineFactory.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"

FRBMKShaderTerrain::FRBMKShaderTerrain(FRBMKShadersManager* InOwner): FRBMKShaderBase(InOwner)
{
	RTextureName = TEXT("detail\\detail_grnd_grass");
	GTextureName = TEXT("detail\\detail_grnd_asphalt");
	BTextureName = TEXT("detail\\detail_grnd_earth");
	ATextureName = TEXT("detail\\detail_grnd_yantar");
}

FRBMKShaderTerrain::~FRBMKShaderTerrain()
{

}

void FRBMKShaderTerrain::Serialize(FArchive& Ar)
{
	FRBMKShaderBase::Serialize(Ar);
	SerializeMaker(Ar,ERBMKShaderPropertiesType::Marker);
	SerializeTexture(Ar,DetailTextureName);
	SkipMatrix(Ar);
	if(Description.Version>=3)
	{
		SerializeTexture(Ar,RTextureName);
		SerializeTexture(Ar,GTextureName);
		SerializeTexture(Ar,BTextureName);
		SerializeTexture(Ar,ATextureName);
	}
}

UMaterialInterface* FRBMKShaderTerrain::ImportSurface(const FString& Path, const FString& TextureName,	const FString& GameMaterial, bool HudMode)
{
	const FString MaskTextureName = TextureName + TEXT("_mask");
	const UStalkerGameSettings* StalkerGameSettings = GetDefault<UStalkerGameSettings>();
	if(UMaterialInterface*TerrainMaster = StalkerGameSettings->DefaultTerrainMaterial.LoadSynchronous())
	{
		const FString ParentName = Description.Name.Replace(TEXT("\\"), TEXT("/"));
		const FString GlobalMaterialInstanceName =  UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("MaterialsInstance") / ParentName/( FPaths::ChangeExtension(TextureName, TEXT("")).Replace(TEXT("\\"), TEXT("/"))));

		UMaterialInstanceConstant* NewGlobalMaterialInstance = nullptr;
		if(!FRBMKEngineFactory::LoadOrCreateOrOverwriteAsset(GlobalMaterialInstanceName,Owner->Owner->GetObjectFlags(),NewGlobalMaterialInstance))
		{
			return NewGlobalMaterialInstance;
		}
		FAssetRegistryModule::AssetCreated(NewGlobalMaterialInstance);
		NewGlobalMaterialInstance->Parent = TerrainMaster;
		NewGlobalMaterialInstance->PhysMaterial = GetPhysicalMaterial(GameMaterial);

		FRBMKShaderTextureInfo BaseTexture = Owner->GetOrImportTexture(TextureName);
		if(UTexture2D*Texture = BaseTexture.BaseTexture.LoadSynchronous())
		{
			NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Base")), Texture);
		}

		FRBMKShaderTextureInfo MaskTexture = Owner->GetOrImportTexture(MaskTextureName);
		if(UTexture2D*Texture = MaskTexture.BaseTexture.LoadSynchronous())
		{
			NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Mask")), Texture);
		}

		const FRBMKShaderTextureInfo DetailTexture1 = Owner->GetOrImportTexture(RTextureName);
		if(UTexture2D*Texture = DetailTexture1.BaseTexture.LoadSynchronous())
		{
			NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Detail_1")), Texture);
		}
		if(DetailTexture1.UseBump)
		{
			if(UTexture2D*Texture = DetailTexture1.NormalMapTexture.LoadSynchronous())
			{
				NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Detail_1_Normal")), Texture);
			}
			if(UTexture2D*Texture = DetailTexture1.HeightGlossTexture.LoadSynchronous())
			{
				NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Detail_1_HeightGloss")), Texture);
			}
		}

		const FRBMKShaderTextureInfo DetailTexture2 = Owner->GetOrImportTexture(GTextureName);
		if(UTexture2D*Texture = DetailTexture2.BaseTexture.LoadSynchronous())
		{
			NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Detail_2")), Texture);
		}
		if(DetailTexture2.UseBump)
		{
			if(UTexture2D*Texture = DetailTexture2.NormalMapTexture.LoadSynchronous())
			{
				NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Detail_2_Normal")), Texture);
			}
			if(UTexture2D*Texture = DetailTexture2.HeightGlossTexture.LoadSynchronous())
			{
				NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Detail_2_HeightGloss")), Texture);
			}
		}

		const FRBMKShaderTextureInfo DetailTexture3 = Owner->GetOrImportTexture(BTextureName);
		if(UTexture2D*Texture = DetailTexture3.BaseTexture.LoadSynchronous())
		{
			NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Detail_3")), Texture);
		}
		if(DetailTexture3.UseBump)
		{
			if(UTexture2D*Texture = DetailTexture3.NormalMapTexture.LoadSynchronous())
			{
				NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Detail_3_Normal")), Texture);
			}
			if(UTexture2D*Texture = DetailTexture3.HeightGlossTexture.LoadSynchronous())
			{
				NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Detail_3_HeightGloss")), Texture);
			}
		}

		const FRBMKShaderTextureInfo DetailTexture4 = Owner->GetOrImportTexture(ATextureName);
		if(UTexture2D*Texture = DetailTexture4.BaseTexture.LoadSynchronous())
		{
			NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Detail_4")), Texture);
		}
		if(DetailTexture4.UseBump)
		{
			if(UTexture2D*Texture = DetailTexture4.NormalMapTexture.LoadSynchronous())
			{
				NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Detail_4_Normal")), Texture);
			}
			if(UTexture2D*Texture = DetailTexture4.HeightGlossTexture.LoadSynchronous())
			{
				NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Detail_4_HeightGloss")), Texture);
			}
		}
		
		NewGlobalMaterialInstance->Modify();
		NewGlobalMaterialInstance->PostEditChange();
		return NewGlobalMaterialInstance;
	}
	else
	{
		return FRBMKShaderBase::ImportSurface(Path, TextureName, GameMaterial, HudMode);
	}
}
