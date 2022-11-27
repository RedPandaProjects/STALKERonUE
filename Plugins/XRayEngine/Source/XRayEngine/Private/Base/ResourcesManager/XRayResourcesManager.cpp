#include "XRayResourcesManager.h"
#include "SkeletonMesh/XRaySkeletonMesh.h"
#include "../EngineManager/XRayEngineManager.h"



USlateBrushAsset* UXRayResourcesManager::GetBrush(FName InNameMaterial, FName InNameTexture)
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
	UE_LOG(LogXRayEngine, Log, TEXT("Create slate brush:[%s]%s"), *NameMaterial,*NameTexture);

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
		const FString ParentPackageName = TEXT("/Game/Materials") / NameMaterial;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	if (!IsValid(ParentMaterial))
	{
		const FString ParentPackageName = TEXT("/XRayEngine/Materials") / NameMaterial;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	if (!IsValid(ParentMaterial))
	{
		UE_LOG(LogXRayEngine, Warning, TEXT("Can't found ui material:%s"), *NameMaterial);
		UMaterialInterface* UnkownMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/XRayEngine/Materials/UIUnkown.UIUnkown"));
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
			const FString ParentPackageName = TEXT("/Game/Textures") / NameTexture;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Texture = LoadObject<UTexture2D>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
		}
		if (!IsValid(Texture))
		{
			const FString ParentPackageName = TEXT("/XRayEngine/Textures") / NameTexture;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Texture = LoadObject<UTexture2D>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
		}
		if (!IsValid(Texture))
		{
			UE_LOG(LogXRayEngine, Warning, TEXT("Can't found texture:%s"), *NameTexture);
			UTexture2D* UnkownTextuer = LoadObject<UTexture2D>(nullptr, TEXT("/XRayEngine/Textures/Unkown.Unkown"));
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

UFont* UXRayResourcesManager::GetFont(FName Name)
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
		const FString ParentPackageName = TEXT("/XRayEngine/Fonts") / NameFont;
		FontPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
	}

	UFont*  Font = LoadObject<UFont>(nullptr, *FontPath, nullptr, LOAD_NoWarn);
	if (!IsValid(Font))
	{	
		if (DefaultFontPath != FontPath)
		{
			UE_LOG(LogXRayEngine, Warning, TEXT("Can't found font:%s"), *Name.ToString());
		}
		Font = LoadObject<UFont>(nullptr, *DefaultFontPath);
		check(IsValid(Font));
	}
	Fonts.Add(Name,Font);
	return Font;
}

void UXRayResourcesManager::Free(USlateBrushAsset* Brush)
{
	int32*Counter = BrushesCounter.Find(Brush);
	check(Counter);
	if (--(*Counter) == 0)
	{
		UE_LOG(LogXRayEngine, Log, TEXT("Destroy slate brush:[%s]%s"), *BrushesInfo[Brush].Matrrial.ToString(), *BrushesInfo[Brush].Texture.ToString());
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

USlateBrushAsset* UXRayResourcesManager::Copy(USlateBrushAsset* Brush)
{
	int32* Counter = BrushesCounter.Find(Brush);
	check(Counter);
	(*Counter)++;
	return Brush;
}

void UXRayResourcesManager::CheckLeak()
{
	check(BrushesNeedReloading.Num() == 0);
	check(Brushes.Num() == 0);
	check(BrushesCounter.Num() == 0);
	check(BrushesMaterials.Num() == 0);
	check(BrushesInfo.Num() == 0);
	check(Meshes.Num() == 0);
}

void UXRayResourcesManager::Reload()
{
	for (auto& [Key, Data] : BrushesCounter)
	{
		BrushesNeedReloading.FindOrAdd(Key);
	}
}

class AXRaySkeletonMesh* UXRayResourcesManager::SpawnSkeletonMesh(class XRayKinematics* Kinematics)
{
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.ObjectFlags = EObjectFlags::RF_Transient;
	AXRaySkeletonMesh* Result = GXRayEngineManager->GetGameWorld()->SpawnActor< AXRaySkeletonMesh>(SpawnParameters);
	Result->SetKinematics(Kinematics);
	Meshes.Add(Result);
	return Result;
}

void UXRayResourcesManager::Destroy(AXRaySkeletonMesh* Mesh)
{
	checkSlow(Meshes.Contains(Mesh));
	Meshes.Remove(Mesh);
	Mesh->Destroy();

}
