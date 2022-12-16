#include "Resources/StalkerResourcesManager.h"
#include "../Kernel/StalkerEngineManager.h"
#include "../Entities/Kinematics/StalkerKinematicsLegacy.h"
#include "../Entities/Kinematics/StalkerKinematics.h"
#include "SkeletonMesh/StalkerKinematicsData.h"
#include "../Entities/Levels/Light/StalkerLight.h"


USlateBrushAsset* UStalkerResourcesManager::GetBrush(FName InNameMaterial, FName InNameTexture)
{
	bool NeedReload = false;
	check(InNameMaterial!=NAME_None);
	if (Brushes.Find(InNameMaterial))
	{
		if (Brushes[InNameMaterial].Find(InNameTexture))
		{

			
			USlateBrushAsset*  Result = Brushes[InNameMaterial][InNameTexture];
			if (BrushesNeedReloading.Find(Result))
			{
				NeedReload = true;
			}
			else
			{
				BrushesCounter[Result]++;
				return Result;
			}
		}
	}

	FString NameTexture = FPaths::GetBaseFilename(InNameTexture.ToString(), false);
	NameTexture.ReplaceCharInline(TEXT('\\'), TEXT('/'));

	FString	NameMaterial = FPaths::GetBaseFilename(InNameMaterial.ToString(), false);
	NameMaterial.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	UE_LOG(LogStalker, Log, TEXT("Create slate brush:[%s]%s"), *NameMaterial,*NameTexture);

	UMaterialInterface* ParentMaterial = nullptr;

	switch (xrGameManager::GetGame())
	{
	default:
	{
		const FString ParentPackageName = TEXT("/Game/COP/Materials") / NameMaterial;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
		break;
	case EGame::CS:
	{
		const FString ParentPackageName = TEXT("/Game/CS/Materials") / NameMaterial;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
		break;
	case EGame::SHOC:
	{
		const FString ParentPackageName = TEXT("/Game/SHOC/Materials") / NameMaterial;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath,nullptr, LOAD_NoWarn);
	}
		break;
	}

	if (!IsValid(ParentMaterial))
	{
		const FString ParentPackageName = TEXT("/Game/Base/Materials") / NameMaterial;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	if (!IsValid(ParentMaterial))
	{
		UE_LOG(LogStalker, Warning, TEXT("Can't found ui material:%s"), *NameMaterial);
		UMaterialInterface* UnkownMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Base/Materials/UIUnkown.UIUnkown"));
		check(IsValid(UnkownMaterial));
		ParentMaterial = UnkownMaterial;
	}

	UTexture2D* Texture = nullptr;
	if(InNameTexture!=NAME_None)
	{
		
		switch (xrGameManager::GetGame())
		{
		default:
		{
			const FString ParentPackageName = TEXT("/Game/COP/Textures") / NameTexture;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Texture = LoadObject<UTexture2D>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
		}
			break;
		case EGame::CS:
		{
			const FString ParentPackageName = TEXT("/Game/CS/Textures") / NameTexture;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Texture = LoadObject<UTexture2D>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
		}
			break;
		case EGame::SHOC:
		{
			const FString ParentPackageName = TEXT("/Game/SHOC/Textures") / NameTexture;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Texture = LoadObject<UTexture2D>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
		}
			break;
		}
		if (!IsValid(Texture))
		{
			const FString ParentPackageName = TEXT("/Game/Base/Textures") / NameTexture;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Texture = LoadObject<UTexture2D>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
		}
		if (!IsValid(Texture))
		{
			UE_LOG(LogStalker, Warning, TEXT("Can't found texture:%s"), *NameTexture);
			UTexture2D* UnkownTextuer = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Base/Textures/Unkown.Unkown"));
			check(IsValid(UnkownTextuer));
			Texture = UnkownTextuer;
		}
	}
	UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(ParentMaterial, this,NAME_None);
	Material->SetFlags(RF_Transient);
	if (InNameTexture != NAME_None)
	{
		Material->SetTextureParameterValue(TEXT("Base"), Texture);
	}
	if (NeedReload)
	{
		USlateBrushAsset* NewSlateBrushAsset = Brushes[InNameMaterial][InNameTexture];
		NewSlateBrushAsset->SetFlags(RF_Transient);
		FVector2D TextureSize = FVector2D(32, 32);
		if (InNameTexture != NAME_None)
		{
			TextureSize = Texture->GetImportedSize();
		}
		NewSlateBrushAsset->Brush = FSlateMaterialBrush(*Material, TextureSize);
		BrushesNeedReloading.Remove(NewSlateBrushAsset);
		BrushesCounter[NewSlateBrushAsset]++;
		return NewSlateBrushAsset;
	}
	USlateBrushAsset* NewSlateBrushAsset = NewObject<USlateBrushAsset>(this);
	NewSlateBrushAsset->SetFlags(RF_Transient);
	FVector2D TextureSize = FVector2D(32,32);
	if (InNameTexture != NAME_None)
	{
		TextureSize = Texture->GetImportedSize();
	}
	NewSlateBrushAsset->Brush =  FSlateMaterialBrush(*Material, TextureSize);
	Brushes.FindOrAdd(InNameMaterial).Add(InNameTexture, NewSlateBrushAsset);
	BrushesCounter.Add(NewSlateBrushAsset,1);
	BrushesMaterials.Add(NewSlateBrushAsset, Material);
	BrushInfo Info ={ InNameMaterial ,InNameTexture };
	BrushesInfo.Add(NewSlateBrushAsset, Info);
	return NewSlateBrushAsset;
}

UFont* UStalkerResourcesManager::GetFont(FName Name)
{
	if (Fonts.Contains(Name))
	{
		return Fonts[Name];
	}
	FString FontPath;
	FString DefaultFontPath = TEXT("/Engine/EngineFonts/Roboto.Roboto");
	FontPath = DefaultFontPath;
	if (Name != TEXT("default"))
	{
		const FString NameFont = Name.ToString();
		const FString ParentPackageName = TEXT("/Game/Base/Fonts") / NameFont;
		FontPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
	}

	UFont*  Font = LoadObject<UFont>(nullptr, *FontPath, nullptr, LOAD_NoWarn);
	if (!IsValid(Font))
	{	
		if (DefaultFontPath != FontPath)
		{
			UE_LOG(LogStalker, Warning, TEXT("Can't found font:%s"), *Name.ToString());
		}
		Font = LoadObject<UFont>(nullptr, *DefaultFontPath);
		check(IsValid(Font));
	}
	Fonts.Add(Name,Font);
	return Font;
}

void UStalkerResourcesManager::Free(USlateBrushAsset* Brush)
{
	int32*Counter = BrushesCounter.Find(Brush);
	check(Counter);
	if (--(*Counter) == 0)
	{
		UE_LOG(LogStalker, Log, TEXT("Destroy slate brush:[%s]%s"), *BrushesInfo[Brush].Matrrial.ToString(), *BrushesInfo[Brush].Texture.ToString());
		BrushesCounter.Remove(Brush);
		BrushesNeedReloading.Remove(Brush);
		BrushesMaterials.Remove(Brush);
		Brushes[BrushesInfo[Brush].Matrrial].Remove(BrushesInfo[Brush].Texture);
		if (Brushes[BrushesInfo[Brush].Matrrial].Num() == 0)
		{
			Brushes.Remove(BrushesInfo[Brush].Matrrial);
		}
		BrushesInfo.Remove(Brush);
	}
}

USlateBrushAsset* UStalkerResourcesManager::Copy(USlateBrushAsset* Brush)
{
	int32* Counter = BrushesCounter.Find(Brush);
	check(Counter);
	(*Counter)++;
	return Brush;
}

void UStalkerResourcesManager::CheckLeak()
{
	check(BrushesNeedReloading.Num() == 0);
	check(Brushes.Num() == 0);
	check(BrushesCounter.Num() == 0);
	check(BrushesMaterials.Num() == 0);
	check(BrushesInfo.Num() == 0);
	check(Meshes.Num() == 0);
	check(MeshesLegacy.Num() == 0);
	check(Lights.Num() == 0);
}

void UStalkerResourcesManager::Reload()
{
	for (auto& [Key, Data] : BrushesCounter)
	{
		BrushesNeedReloading.FindOrAdd(Key);
	}
}


class AStalkerLight* UStalkerResourcesManager::CreateLight()
{
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.ObjectFlags = EObjectFlags::RF_Transient;
	AStalkerLight* Result = GXRayEngineManager->GetGameWorld()->SpawnActor< AStalkerLight>(SpawnParameters);
	Lights.Add(Result);
	return Result;
}

void UStalkerResourcesManager::Desotry(class IRender_Light* InLight)
{
	AStalkerLight*Light =  static_cast<AStalkerLight*>(InLight);
	checkSlow(Lights.Contains(Light));
	Lights.Remove(Light);
	Light->Destroy();
}

class AStalkerKinematics* UStalkerResourcesManager::CreateKinematics(class UStalkerKinematicsData* KinematicsData)
{
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.ObjectFlags = EObjectFlags::RF_Transient;
	AStalkerKinematics* Result = GXRayEngineManager->GetGameWorld()->SpawnActor< AStalkerKinematics>(SpawnParameters);
	Result->Initilize(KinematicsData);
	return Result;
}

class AStalkerKinematics* UStalkerResourcesManager::CreateKinematics(const char* InName)
{
	UStalkerKinematicsData* KinematicsData = nullptr;
	FString Name = InName;
	Name.ReplaceInline(TEXT("\\"), TEXT("/"));
	Name.ReplaceInline(TEXT("#"), TEXT("_"));
	switch (xrGameManager::GetGame())
	{
	default:
	{
		const FString ParentPackageName = TEXT("/Game/COP/Meshes") / Name;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		KinematicsData = LoadObject<UStalkerKinematicsData>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
		if (!IsValid(KinematicsData))
		{
			if (Name.StartsWith(TEXT("meshes/")))
			{
				FString NewName = Name;
				NewName.RemoveFromStart(TEXT("meshes/"));
				const FString ParentPackageNameFromLevel = TEXT("/Game/COP/Meshes/levels")/GXRayEngineManager->GetCurrentLevel().ToString() / NewName;
				const FString ParentObjectPathFromLevel = ParentPackageNameFromLevel + TEXT(".") + FPaths::GetBaseFilename(ParentPackageNameFromLevel);
				KinematicsData = LoadObject<UStalkerKinematicsData>(nullptr, *ParentObjectPathFromLevel, nullptr, LOAD_NoWarn);
			}
		}
	}
	break;
	case EGame::CS:
	{
		const FString ParentPackageName = TEXT("/Game/CS/Meshes") / Name;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		KinematicsData = LoadObject<UStalkerKinematicsData>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
		if (!IsValid(KinematicsData))
		{
			if (Name.StartsWith(TEXT("meshes/")))
			{
				FString NewName = Name;
				NewName.RemoveFromStart(TEXT("meshes/"));
				const FString ParentPackageNameFromLevel = TEXT("/Game/CS/Meshes/levels") / GXRayEngineManager->GetCurrentLevel().ToString() / NewName;
				const FString ParentObjectPathFromLevel = ParentPackageNameFromLevel + TEXT(".") + FPaths::GetBaseFilename(ParentPackageNameFromLevel);
				KinematicsData = LoadObject<UStalkerKinematicsData>(nullptr, *ParentObjectPathFromLevel, nullptr, LOAD_NoWarn);
			}
		}
	}
	break;
	case EGame::SHOC:
	{
		const FString ParentPackageName = TEXT("/Game/SHOC/Meshes") / Name;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		KinematicsData = LoadObject<UStalkerKinematicsData>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
		if (!IsValid(KinematicsData))
		{
			if (Name.StartsWith(TEXT("meshes/")))
			{
				FString NewName = Name;
				NewName.RemoveFromStart(TEXT("meshes/"));
				const FString ParentPackageNameFromLevel = TEXT("/Game/SHOC/Meshes/levels") / GXRayEngineManager->GetCurrentLevel().ToString() / NewName;
				const FString ParentObjectPathFromLevel = ParentPackageNameFromLevel + TEXT(".") + FPaths::GetBaseFilename(ParentPackageNameFromLevel);
				KinematicsData = LoadObject<UStalkerKinematicsData>(nullptr, *ParentObjectPathFromLevel, nullptr, LOAD_NoWarn);
			}
		}
	}
	break;
	}
	if (!IsValid(KinematicsData))
	{
		const FString ParentPackageName = TEXT("/Game/Base/Meshes") / Name;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		KinematicsData = LoadObject<UStalkerKinematicsData>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	if (IsValid(KinematicsData)&& IsValid(KinematicsData->Mesh))
	{
		class AStalkerKinematics*Result= CreateKinematics(KinematicsData);
		Meshes.Add(Result);
		return Result;
	}
	check(false);
	return nullptr;
}

class AStalkerKinematicsLegacy* UStalkerResourcesManager::SpawnSkeletonMesh(class XRayKinematicsLegacy* Kinematics)
{
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.ObjectFlags = EObjectFlags::RF_Transient;
	AStalkerKinematicsLegacy* Result = GXRayEngineManager->GetGameWorld()->SpawnActor< AStalkerKinematicsLegacy>(SpawnParameters);
	Result->SetKinematics(Kinematics);
	MeshesLegacy.Add(Result);
	return Result;
}

void UStalkerResourcesManager::Destroy(AStalkerKinematicsLegacy* Mesh)
{
	checkSlow(MeshesLegacy.Contains(Mesh));
	MeshesLegacy.Remove(Mesh);
	Mesh->Destroy();

}

void UStalkerResourcesManager::Destroy(AStalkerKinematics* Mesh)
{
	checkSlow(Meshes.Contains(Mesh));
	Meshes.Remove(Mesh);
	Mesh->Destroy();
}
