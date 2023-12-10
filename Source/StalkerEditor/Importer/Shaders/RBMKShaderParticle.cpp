#include "RBMKShaderParticle.h"

#include "StalkerEditorManager.h"
#include "Importer/FRBMKEngineFactory.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"

FRBMKShaderParticle::FRBMKShaderParticle(FRBMKShadersManager* Owner): FRBMKShaderBase(Owner){}

FRBMKShaderParticle::FRBMKShaderParticle(const FString& Name, FRBMKShadersManager* Owner): FRBMKShaderBase(Owner)
{
	Description.Name = Name;
}

UMaterialInterface* FRBMKShaderParticle::ImportSurface(const FString& Path, const FString& TextureName,	const FString& GameMaterial, bool HudMode)
{
	const FString ParentName = Description.Name.Replace(TEXT("\\"), TEXT("/"));
	const FString GlobalMaterialInstanceName =  UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("MaterialsInstance") / ParentName/( FPaths::ChangeExtension(TextureName, TEXT("")).Replace(TEXT("\\"), TEXT("/"))));

	UMaterialInstanceConstant* MaterialInstanceConstant = nullptr;
	if(FRBMKEngineFactory::LoadOrCreateOrOverwriteAsset(GlobalMaterialInstanceName,Owner->Owner->GetObjectFlags(),MaterialInstanceConstant))
	{
		FAssetRegistryModule::AssetCreated(MaterialInstanceConstant);

		const UStalkerGameSettings* StalkerGameSettings = GetDefault<UStalkerGameSettings>();
		MaterialInstanceConstant->Parent = Owner->GetOrCreateMasterMaterial(Description.Name,StalkerGameSettings->DefaultParticleUnknownMaterial);

		TArray<FString> TexturesNames;
		if (TextureName.ParseIntoArray(TexturesNames, TEXT(",")) > 2)
		{
			UE_LOG(LogXRayImporter, Warning, TEXT("Exceeded texture stack[%s] in %s "),*TextureName,*GlobalMaterialInstanceName);
		}

		for (int32 i = 0; i < 2; i++)
		{
			if (TexturesNames.Num() <= i)
			{
				continue;
			}
			FRBMKShaderTextureInfo BaseTexture = Owner->GetOrImportTexture(TexturesNames[i]);
			if (UTexture2D*Texture =  BaseTexture.BaseTexture.LoadSynchronous())
			{
				FString CurrentTextureName = FString::Printf(TEXT("Base%d"),i);
				MaterialInstanceConstant->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(*CurrentTextureName), Texture);
			}
		}

		MaterialInstanceConstant->Modify();
		MaterialInstanceConstant->PostEditChange();
	}
	
	if (HudMode)
	{
		const FString GlobalMaterialInstanceNameForHUD = GlobalMaterialInstanceName + TEXT("_HUD");
		UMaterialInstanceConstant* MaterialInstanceConstantHUD = nullptr;
		if(FRBMKEngineFactory::LoadOrCreateOrOverwriteAsset(GlobalMaterialInstanceNameForHUD,Owner->Owner->GetObjectFlags(),MaterialInstanceConstantHUD))
		{
			FAssetRegistryModule::AssetCreated(MaterialInstanceConstantHUD);
			MaterialInstanceConstantHUD->Parent = MaterialInstanceConstant;

			FStaticParameterSet NewStaticParameterSet;
			FStaticSwitchParameter SwitchParameter;
			SwitchParameter.ParameterInfo.Name = TEXT("HUD Mode");
			SwitchParameter.Value = true;
			SwitchParameter.bOverride = true;
			NewStaticParameterSet.StaticSwitchParameters.Add(SwitchParameter);
			MaterialInstanceConstantHUD->UpdateStaticPermutation(NewStaticParameterSet);
			MaterialInstanceConstantHUD->InitStaticPermutation();

			MaterialInstanceConstantHUD->Modify();
			MaterialInstanceConstantHUD->PostEditChange();
		}
		return MaterialInstanceConstantHUD;
	}

	return MaterialInstanceConstant;
	
	
}
