#include "XRayResourcesManager.h"

UXRayResourcesManager* GXRayResourcesManager = nullptr;

USlateBrushAsset* UXRayResourcesManager::GetBrush(FName InNameMaterial, FName InNameTexture)
{
	if (Brushes.Find(InNameMaterial))
	{
		if (Brushes[InNameMaterial].Find(InNameTexture))
		{
			USlateBrushAsset*  Result = Brushes[InNameMaterial][InNameTexture];
			BrushesCounter[Result]++;
			return Result;
		}
	}

	FString NameTexture = FPaths::GetBaseFilename(InNameTexture.ToString(), false);
	NameTexture.ReplaceCharInline(TEXT('\\'), TEXT('/'));

	FString	NameMaterial = FPaths::GetBaseFilename(InNameMaterial.ToString(), false);
	NameMaterial.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	UE_LOG(LogXRayEngine, Log, TEXT("Create slate brush:[%s]%s"), *NameMaterial,*NameTexture);

	UMaterialInterface* ParentMaterial = nullptr;
	{
		const FString ParentPackageName = TEXT("/Game/Materials") / NameMaterial;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath);
	}
	if (!IsValid(ParentMaterial))
	{
		const FString ParentPackageName = TEXT("/XRayEngine/Materials") / NameMaterial;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath);
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
		{
			const FString ParentPackageName = TEXT("/Game/Textures") / NameTexture;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Texture = LoadObject<UTexture2D>(nullptr, *ParentObjectPath);
		}
		if (!IsValid(Texture))
		{
			const FString ParentPackageName = TEXT("/XRayEngine/Textures") / NameTexture;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Texture = LoadObject<UTexture2D>(nullptr, *ParentObjectPath);
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

void UXRayResourcesManager::Free(USlateBrushAsset* Brush)
{
	int32*Counter = BrushesCounter.Find(Brush);
	check(Counter);
	if (--(*Counter) == 0)
	{
		UE_LOG(LogXRayEngine, Log, TEXT("Destroy slate brush:[%s]%s"), *BrushesInfo[Brush].Matrrial.ToString(), *BrushesInfo[Brush].Texture.ToString());
		BrushesCounter.Remove(Brush);
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
