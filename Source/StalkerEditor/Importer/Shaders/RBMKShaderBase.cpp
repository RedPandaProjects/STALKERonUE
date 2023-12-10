#include "RBMKShaderBase.h"

#include "RBMKShadersManager.h"
#include "StalkerEditorManager.h"
#include "Importer/FRBMKEngineFactory.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"

FRBMKShaderBase::FRBMKShaderBase(FRBMKShadersManager* InOwner):Owner(InOwner)
{
}

FRBMKShaderBase::~FRBMKShaderBase()
{
}

void FRBMKShaderBase::Serialize(FArchive& Ar)
{
	Ar<<Description;
	SerializeMaker(Ar,ERBMKShaderPropertiesType::Marker);
	SerializeInteger(Ar,Priority);
	SerializeBool(Ar,StrictSorting);
	SerializeMaker(Ar,ERBMKShaderPropertiesType::Marker);
	SerializeTexture(Ar,MainTexture);
	SkipMatrix(Ar);
}

UMaterialInterface* FRBMKShaderBase::ImportSurface(const FString& Path, const FString& TextureName, const FString& GameMaterial,bool HudMode)
{
	UStalkerPhysicalMaterial* StalkerPhysicalMaterial = GetPhysicalMaterial(GameMaterial);
	
	const UStalkerGameSettings* StalkerGameSettings = GetDefault<UStalkerGameSettings>();

	UMaterialInterface* LocalMaterialInstance = LoadObject<UMaterialInterface>(nullptr, *(Path + TEXT(".") + FPaths::GetBaseFilename(Path)), nullptr, LOAD_NoWarn);
	if (LocalMaterialInstance&&!StalkerGameSettings->AlwaysOverwriteAssetsWhenImport)
	{
		return LocalMaterialInstance;
	}

	const FString ParentName = Description.Name.Replace(TEXT("\\"), TEXT("/"));


	const FString GlobalMaterialInstanceName =  UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("MaterialsInstance") / ParentName/( FPaths::ChangeExtension(TextureName, TEXT("")).Replace(TEXT("\\"), TEXT("/"))));

	UMaterialInterface* GlobalMaterialInstance = LoadObject<UMaterialInterface>(nullptr, *(GlobalMaterialInstanceName + TEXT(".") + FPaths::GetBaseFilename(GlobalMaterialInstanceName)), nullptr, LOAD_NoWarn);

	if (!GlobalMaterialInstance|| (GlobalMaterialInstance->GetPhysicalMaterial() != StalkerPhysicalMaterial && StalkerGameSettings->AlwaysOverwriteAssetsWhenImport))
	{
		UMaterialInstanceConstant* NewGlobalMaterialInstance = nullptr;
		if(!FRBMKEngineFactory::LoadOrCreateOrOverwriteAsset(GlobalMaterialInstanceName,Owner->Owner->GetObjectFlags(),NewGlobalMaterialInstance))
		{
			return nullptr;
		}
		UMaterialInterface* MasterMaterial = Owner->GetOrCreateMasterMaterial(Description.Name, StalkerGameSettings->DefaultUnknownMaterial);
		FAssetRegistryModule::AssetCreated(NewGlobalMaterialInstance);
		NewGlobalMaterialInstance->Parent = MasterMaterial;
		NewGlobalMaterialInstance->PhysMaterial = StalkerPhysicalMaterial;

		FRBMKShaderTextureInfo TextureInfo = Owner->GetOrImportTexture(TextureName);

		FStaticParameterSet NewStaticParameterSet;

		if (UTexture2D*Texture = TextureInfo.BaseTexture.LoadSynchronous())
		{
			NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Diffuse")), Texture);
		}
		{
			NewGlobalMaterialInstance->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Material")), TextureInfo.Material);

			if (TextureInfo.UseBump)
			{
				{
					FStaticSwitchParameter SwitchParameter;
					SwitchParameter.ParameterInfo.Name = TEXT("UseBump");
					SwitchParameter.Value = true;
					SwitchParameter.bOverride = true;
					NewStaticParameterSet.StaticSwitchParameters.Add(SwitchParameter);
				}

				{
					FStaticSwitchParameter SwitchParameter;
					SwitchParameter.ParameterInfo.Name = TEXT("UseParallax");
					SwitchParameter.Value = TextureInfo.UseParallax;
					SwitchParameter.bOverride = true;
					NewStaticParameterSet.StaticSwitchParameters.Add(SwitchParameter);
				}

				if (UTexture2D* NormalMapTexture = TextureInfo.NormalMapTexture.LoadSynchronous())
				{
					NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("NormalMap")), NormalMapTexture);
				}

				if (UTexture2D* HeightGlossTexture = TextureInfo.HeightGlossTexture.LoadSynchronous())
				{
					NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("HeightGloss")), HeightGlossTexture);
				}

				NewGlobalMaterialInstance->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(TEXT("ParallaxHeight")), TextureInfo.ParallaxHeight);
			}

			if (TextureInfo.UseDetail)
			{
				FStaticSwitchParameter SwitchParameter;
				SwitchParameter.ParameterInfo.Name = TEXT("UseDetail");
				SwitchParameter.Value = true;
				SwitchParameter.bOverride = true;
				NewStaticParameterSet.StaticSwitchParameters.Add(SwitchParameter);
				if (UTexture2D* DetailTexture = TextureInfo.DetailTexture.LoadSynchronous())
				{
					NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("DetailDiffuse")), DetailTexture);
				}
				NewGlobalMaterialInstance->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(TEXT("DetailScale")), TextureInfo.DetailScale);
				if (TextureInfo.UseDetailBump)
				{
					SwitchParameter.ParameterInfo.Name = TEXT("UseDetailBump");
					SwitchParameter.Value = true;
					SwitchParameter.bOverride = true;
					NewStaticParameterSet.StaticSwitchParameters.Add(SwitchParameter);

					if (UTexture2D* NormalMapTextureDetail = TextureInfo.DetailNormalMapTexture.LoadSynchronous())
					{
						NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("DetailNormalMap")), NormalMapTextureDetail);
					}

					if (UTexture2D* HeightGlossTextureDetail = TextureInfo.DetailHeightGlossTexture.LoadSynchronous())
					{
						NewGlobalMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("HeightGlossDetail")), HeightGlossTextureDetail);
					}
				}
			}
		}

		NewGlobalMaterialInstance->UpdateStaticPermutation(NewStaticParameterSet);
		NewGlobalMaterialInstance->InitStaticPermutation();
		NewGlobalMaterialInstance->Modify();
		NewGlobalMaterialInstance->PostEditChange();
		if(!HudMode)
		{
			return NewGlobalMaterialInstance;
		}
		GlobalMaterialInstance = NewGlobalMaterialInstance;
	}
	if(GlobalMaterialInstance->GetPhysicalMaterial() != StalkerPhysicalMaterial || HudMode)
	{
		UMaterialInstanceConstant*LocalMaterialInstanceNew = nullptr;
		if(!FRBMKEngineFactory::LoadOrCreateOrOverwriteAsset(Path,Owner->Owner->GetObjectFlags(),LocalMaterialInstanceNew))
		{
			return nullptr;
		}
		FStaticParameterSet NewStaticParameterSet;
		FAssetRegistryModule::AssetCreated(LocalMaterialInstanceNew);
		LocalMaterialInstanceNew->Parent = GlobalMaterialInstance;
		LocalMaterialInstanceNew->PhysMaterial = StalkerPhysicalMaterial;
		if(HudMode)
		{
			FStaticSwitchParameter SwitchParameter;
			SwitchParameter.ParameterInfo.Name = TEXT("HUD Mode");
			SwitchParameter.Value = true;
			SwitchParameter.bOverride = true;
			NewStaticParameterSet.StaticSwitchParameters.Add(SwitchParameter);
		}
		LocalMaterialInstanceNew->UpdateStaticPermutation(NewStaticParameterSet);
		LocalMaterialInstanceNew->InitStaticPermutation();
		LocalMaterialInstanceNew->Modify();
		LocalMaterialInstanceNew->PostEditChange();
		return LocalMaterialInstanceNew;
	}
	return GlobalMaterialInstance;
}

UStalkerPhysicalMaterial* FRBMKShaderBase::GetPhysicalMaterial(const FString& Name)
{
	FString NewName = Name;
	if(NewName.IsEmpty())
	{
		NewName = TEXT("default");
	}
	NewName.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString PhysicalMaterialsPackageName = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("PhysicalMaterials") / TEXT("Materials") / NewName);
	const FString PhysicalMaterialsObjectPath = PhysicalMaterialsPackageName + TEXT(".") + FPaths::GetBaseFilename(PhysicalMaterialsPackageName);
	return LoadObject<UStalkerPhysicalMaterial>(nullptr, *PhysicalMaterialsObjectPath, nullptr, 0);
}

void FRBMKShaderBase::SerializeMaker(FArchive& Ar,ERBMKShaderPropertiesType PropertiesType)
{
	ERBMKShaderPropertiesType InPropertiesType = PropertiesType;
	Ar<<InPropertiesType;
	check(InPropertiesType == PropertiesType);
	FStalkerFileSystem::SkipStringZ(Ar);
}

void FRBMKShaderBase::SkipMatrix(FArchive& Ar)
{
	SerializeMaker(Ar,ERBMKShaderPropertiesType::Matrix);
	char InName[64] = {};
	Ar.Serialize(InName,64);
}

void FRBMKShaderBase::SerializeTexture(FArchive& Ar, FString& Value)
{
	SerializeMaker(Ar,ERBMKShaderPropertiesType::Texture);
	char InName[64] = {};
	Ar.Serialize(InName,64);
	Value = ANSI_TO_TCHAR(InName);
}

void FRBMKShaderBase::SerializeInteger(FArchive& Ar, int32& Value)
{
	SerializeMaker(Ar,ERBMKShaderPropertiesType::Integer);
	int32 Min = 0 ,Max = 0;
	Ar<<Value<<Min<<Max;
}

void FRBMKShaderBase::SerializeFloat(FArchive& Ar, float& Value)
{
	SerializeMaker(Ar,ERBMKShaderPropertiesType::Float);
	float Min = 0 ,Max = 0;
	Ar<<Value<<Min<<Max;
}

void FRBMKShaderBase::SerializeBool(FArchive& Ar, bool& Value)
{
	SerializeMaker(Ar,ERBMKShaderPropertiesType::Bool);
	int32 InValue = Value?1:0;
	Ar<<InValue;
	Value = InValue!=0?true:false;
}
