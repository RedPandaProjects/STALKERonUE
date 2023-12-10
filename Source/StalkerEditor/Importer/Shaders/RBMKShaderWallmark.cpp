#include "RBMKShaderWallmark.h"

#include "StalkerEditorManager.h"
#include "Importer/FRBMKEngineFactory.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"

FRBMKShaderWallmark::FRBMKShaderWallmark(const FString& Name, FRBMKShadersManager* Owner):FRBMKShaderBase(Owner)
{
	Description.Name = Name;
}

UMaterialInterface* FRBMKShaderWallmark::ImportSurface(const FString& Path, const FString& TextureName, const FString& GameMaterial, bool HudMode)
{
	const FString ParentName = Path.Replace(TEXT("\\"), TEXT("/"));
	const FString GlobalMaterialInstanceName =  UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("MaterialsInstance") / ParentName/( FPaths::ChangeExtension(TextureName, TEXT("")).Replace(TEXT("\\"), TEXT("/"))));

	UMaterialInstanceConstant* MaterialInstanceName = nullptr;
	if(FRBMKEngineFactory::LoadOrCreateOrOverwriteAsset(GlobalMaterialInstanceName,Owner->Owner->GetObjectFlags(),MaterialInstanceName))
	{
		const UStalkerGameSettings* StalkerGameSettings = GetDefault<UStalkerGameSettings>();

		FAssetRegistryModule::AssetCreated(MaterialInstanceName);
		MaterialInstanceName->Parent = Owner->GetOrCreateMasterMaterial(Description.Name, StalkerGameSettings->DefaultDecalUnknownMaterial);;

		const FRBMKShaderTextureInfo TextureInfo = Owner->GetOrImportTexture(TextureName);

		if (UTexture2D*Texture = TextureInfo.BaseTexture.LoadSynchronous())
		{
			static FName NAME_Diffuse = "Diffuse";
			MaterialInstanceName->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(NAME_Diffuse), Texture);
		}

		MaterialInstanceName->Modify();
		MaterialInstanceName->PostEditChange();
	}

	return MaterialInstanceName;
}
