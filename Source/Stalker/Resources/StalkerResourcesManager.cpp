#include "Resources/StalkerResourcesManager.h"
#include "../Kernel/StalkerEngineManager.h"
#include "../Entities/Kinematics/StalkerKinematicsComponent.h"
#include "SkeletonMesh/StalkerKinematicsData.h"
#include "../Entities/Levels/Light/StalkerLight.h"
#include "../Entities/Levels/Decal/StalkerDecal.h"
#include "../Entities/Levels/Proxy/StalkerProxy.h"
#include "Entities/Player/Character/StalkerPlayerCharacter.h"
#include "Spawn/StalkerGameSpawn.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/xr_object.h"
THIRD_PARTY_INCLUDES_END

FStalkerResourcesManager::ShaderMaterialInfo* FStalkerResourcesManager::GetShaderMaterial(FName InNameMaterial, FName InNameTexture) {
	
	bool NeedReload = false;
	check(InNameMaterial!=NAME_None);

	ShaderMaterialInfo *materialInfo = nullptr;

	if (auto *texturesMap = ShaderMaterials.Find(InNameMaterial))
	{
		ShaderMaterialInfo **ppMaterialInfo = texturesMap->Find(InNameTexture);
		if (ppMaterialInfo)
		{
			materialInfo = *ppMaterialInfo;
			if (materialInfo->NeedsReloading) {
				NeedReload = true;
			} else {
				materialInfo->RefCount++;
				return materialInfo;
			}
		}
	}

	FString NameTexture = FPaths::GetBaseFilename(InNameTexture.ToString(), false);
	NameTexture.ReplaceCharInline(TEXT('\\'), TEXT('/'));

	FString	NameMaterial = FPaths::GetBaseFilename(InNameMaterial.ToString(), false);
	NameMaterial.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	UE_LOG(LogStalker, Log, TEXT("Load shader material:[%s]%s"), *NameMaterial,*NameTexture);

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
		UE_LOG(LogStalker, Warning, TEXT("Can't found shader material:%s"), *NameMaterial);
		UMaterialInterface* UnkownMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Base/Materials/UIUnkown.UIUnkown"));
		check(IsValid(UnkownMaterial));
		ParentMaterial = UnkownMaterial;
	}

	UTexture* Texture = nullptr;
	if(InNameTexture!=NAME_None)
	{
		
		switch (xrGameManager::GetGame())
		{
		default:
		{
			const FString ParentPackageName = TEXT("/Game/COP/Textures") / NameTexture;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Texture = LoadObject<UTexture>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
		}
			break;
		case EGame::CS:
		{
			const FString ParentPackageName = TEXT("/Game/CS/Textures") / NameTexture;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Texture = LoadObject<UTexture>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
		}
			break;
		case EGame::SHOC:
		{
			const FString ParentPackageName = TEXT("/Game/SHOC/Textures") / NameTexture;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Texture = LoadObject<UTexture>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
		}
			break;
		}
		if (!IsValid(Texture))
		{
			const FString ParentPackageName = TEXT("/Game/Base/Textures") / NameTexture;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Texture = LoadObject<UTexture>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
		}
		if (!IsValid(Texture))
		{
			UE_LOG(LogStalker, Warning, TEXT("Can't found texture:%s"), *NameTexture);
			UTexture* UnkownTextuer = LoadObject<UTexture>(nullptr, TEXT("/Game/Base/Textures/Unkown.Unkown"));
			check(IsValid(UnkownTextuer));
			Texture = UnkownTextuer;
		}
	}
	UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(ParentMaterial,nullptr,NAME_None);
	Material->SetFlags(RF_Transient);
	if (InNameTexture != NAME_None)
	{
		Material->SetTextureParameterValue(TEXT("Base"), Texture);
	}
	
	if (NeedReload)
	{
		materialInfo->Material = Material;
		materialInfo->NeedsReloading = false;
		materialInfo->RefCount++;
		return materialInfo;
	}
	materialInfo = new ShaderMaterialInfo;
	materialInfo->Material = Material;
	materialInfo->Texture = Texture;
	materialInfo->MaterialName = InNameMaterial;
	materialInfo->TextureName = InNameTexture;
	ShaderMaterials.FindOrAdd(InNameMaterial).Add(InNameTexture, materialInfo);
	return materialInfo;
}

void FStalkerResourcesManager::Free(ShaderMaterialInfo* MaterialInfo) {
	if (--MaterialInfo->RefCount == 0)
	{
		UE_LOG(LogStalker, Log, TEXT("Release shader material:[%s]%s"), *MaterialInfo->MaterialName.ToString(), *MaterialInfo->TextureName.ToString());

		auto *map = ShaderMaterials.Find(MaterialInfo->MaterialName);
		check(map);
		if (map) {
			map->Remove(MaterialInfo->TextureName);
		}
		if (map->IsEmpty()) {
			ShaderMaterials.Remove(MaterialInfo->MaterialName);
		}
		delete MaterialInfo;
	}
}

FStalkerResourcesManager::ShaderMaterialInfo* FStalkerResourcesManager::Copy(ShaderMaterialInfo* MaterialInfo) {
	MaterialInfo->RefCount++;
	return MaterialInfo;
}

UTexture* FStalkerResourcesManager::FindBrushTexture(USlateBrushAsset* Brush) {
	if (auto *ppBrushInfo = BrushInfoRefs.Find(Brush)) {
		if (auto *material = (*ppBrushInfo)->Material) {
			return material->Texture;
		}
	}
	return nullptr;
}

USlateBrushAsset* FStalkerResourcesManager::GetBrush(ShaderMaterialInfo *Material)
{
	bool NeedReload = false;
	check(Material != nullptr);

	BrushInfo *pBrushInfo = nullptr;
	BrushInfo **ppBrushInfo = Brushes.Find(Material);

	if (ppBrushInfo)
	{
		pBrushInfo = *ppBrushInfo;
#if WITH_EDITORONLY_DATA
		if (pBrushInfo->NeedsReloading)
		{
			NeedReload = true;
		}
		else
#endif
		{
			pBrushInfo->RefCount++;
			return pBrushInfo->Brush;
		}
	}
	if (NeedReload && pBrushInfo)
	{
		USlateBrushAsset* NewSlateBrushAsset = pBrushInfo->Brush;
		NewSlateBrushAsset->SetFlags(RF_Transient);
		FVector2D TextureSize = FVector2D(32, 32);
		NewSlateBrushAsset->Brush = FSlateMaterialBrush(*Material->GetMaterial(), TextureSize);
#if WITH_EDITORONLY_DATA
		pBrushInfo->NeedsReloading = false;
#endif
		pBrushInfo->RefCount++;
		return NewSlateBrushAsset;
	}
	USlateBrushAsset* NewSlateBrushAsset = NewObject<USlateBrushAsset>();
	NewSlateBrushAsset->SetFlags(RF_Transient);
	FVector2D TextureSize = FVector2D(32,32);
	NewSlateBrushAsset->Brush =  FSlateMaterialBrush(*Material->GetMaterial(), TextureSize);

	pBrushInfo = new BrushInfo;
	pBrushInfo->Brush = NewSlateBrushAsset;
	pBrushInfo->Material = Material;

	Brushes.Add(Material, pBrushInfo);
	BrushInfoRefs.Add(pBrushInfo->Brush, pBrushInfo);
	return NewSlateBrushAsset;
}

UFont* FStalkerResourcesManager::GetFont(FName Name)
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

void FStalkerResourcesManager::Free(USlateBrushAsset* Brush)
{
	auto *BrushInfo = BrushInfoRefs.Find(Brush);
	check(BrushInfo);
	if (--(*BrushInfo)->RefCount == 0)
	{
		// UE_LOG(LogStalker, Log, TEXT("Destroy slate brush:[%s]%s"), *BrushesInfo[Brush].Matrrial.ToString(), *BrushesInfo[Brush].Texture.ToString());
		Brushes.Remove((*BrushInfo)->Material);
		BrushInfoRefs.Remove(Brush);
		delete *BrushInfo;
	}
}

USlateBrushAsset* FStalkerResourcesManager::Copy(USlateBrushAsset* Brush)
{
	auto *info = BrushInfoRefs.Find(Brush);
	check(info);
	(*info)->RefCount++;
	return Brush;
}

void FStalkerResourcesManager::CheckLeak()
{
	check(Meshes.Num() == 0);
}

void FStalkerResourcesManager::Reload()
{
#if WITH_EDITORONLY_DATA
	for (auto& [Key, Data] : Brushes)
	{
		Data->NeedsReloading = true;
	}
	for (auto& [Key, Data] : ShaderMaterials)
	{
		for (auto& [Key2, Data2] : Data)
		{
			Data2->NeedsReloading = true;
		}
	}
#endif
}


class AStalkerLight* FStalkerResourcesManager::CreateLight()
{
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.ObjectFlags = EObjectFlags::RF_Transient;
	AStalkerLight* Result = GWorld->SpawnActor< AStalkerLight>(SpawnParameters);
	Result->Lock();
	return Result;
}

void FStalkerResourcesManager::Desotry(class IRender_Light* InLight)
{
	AStalkerLight*Light =  static_cast<AStalkerLight*>(InLight);
	Light->Unlock();
	Light->Destroy();
}

class AStalkerDecal* FStalkerResourcesManager::CreateDecal()
{
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.ObjectFlags = EObjectFlags::RF_Transient;
	AStalkerDecal* Result = GWorld->SpawnActor<AStalkerDecal>(SpawnParameters);
	return Result;
}

void FStalkerResourcesManager::ClearDecals() {
	TArray<AActor *> ActorArray;
	UGameplayStatics::GetAllActorsOfClass(static_cast<UWorld *>(GWorld), AStalkerDecal::StaticClass(), ActorArray);
	for (AActor *Actor : ActorArray) {
		Actor->Destroy();
	}
}

class UStalkerKinematicsData* FStalkerResourcesManager::GetKinematics(const char* InName)
{
	if (!FApp::IsGame()&&(!InName || !InName[0]))
	{
		return nullptr;
	}

	UStalkerKinematicsData* KinematicsData  = LoadObject<UStalkerKinematicsData>(nullptr, *FString(InName), nullptr, LOAD_NoWarn);;

	FString Name = InName;
	if(!IsValid(KinematicsData))
	{
	
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
					const FString ParentPackageNameFromLevel = TEXT("/Game/COP/Meshes/levels") / GStalkerEngineManager->GetCurrentWorldName() / NewName;
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
					const FString ParentPackageNameFromLevel = TEXT("/Game/CS/Meshes/levels") / GStalkerEngineManager->GetCurrentWorldName() / NewName;
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
					const FString ParentPackageNameFromLevel = TEXT("/Game/SHOC/Meshes/levels") / GStalkerEngineManager->GetCurrentWorldName() / NewName;
					const FString ParentObjectPathFromLevel = ParentPackageNameFromLevel + TEXT(".") + FPaths::GetBaseFilename(ParentPackageNameFromLevel);
					KinematicsData = LoadObject<UStalkerKinematicsData>(nullptr, *ParentObjectPathFromLevel, nullptr, LOAD_NoWarn);
				}
			}
		}
		break;
		}
	}
	if (!IsValid(KinematicsData))
	{
		const FString ParentPackageName = TEXT("/Game/Base/Meshes") / Name;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		KinematicsData = LoadObject<UStalkerKinematicsData>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	if (!ensure(IsValid(KinematicsData))||!ensure(IsValid(KinematicsData->Mesh)))
	{
#ifdef WITH_EDITOR
		FMessageLog StalkerEditorFMessageLog("StalkerEditor");
		if(FApp::IsGame())
		{
			StalkerEditorFMessageLog.Error(FText::FromString(FString::Printf(TEXT("Can't found mesh %S"),InName)));
			StalkerEditorFMessageLog.Open(EMessageSeverity::Error);
		}
#endif
		UE_LOG(LogStalker, Error, TEXT("Can't found mesh %S"),InName);
		KinematicsData = LoadObject<UStalkerKinematicsData>(nullptr,TEXT("/Game/Base/Meshes/Error_KinematicsData.Error_KinematicsData"));
		check(KinematicsData);
		check(KinematicsData->Mesh);
	}
	return KinematicsData;
}

class UStalkerKinematicsComponent* FStalkerResourcesManager::CreateKinematics(class UStalkerKinematicsData* KinematicsData)
{
	UStalkerKinematicsComponent* Result =  NewObject< UStalkerKinematicsComponent>();
	Result->SetFlags(EObjectFlags::RF_Transient);
	Result->Initilize(KinematicsData);
	return Result;
}

class UStalkerKinematicsComponent* FStalkerResourcesManager::CreateKinematics(const char* InName, bool NeedRefence)
{
	UStalkerKinematicsData* KinematicsData = GetKinematics(InName);
	if (IsValid(KinematicsData))
	{
		UStalkerKinematicsComponent* Result = CreateKinematics(KinematicsData);;
		if (NeedRefence)
		{
			Meshes.Add(Result);
		}
		return Result;
	}
	return nullptr;
}


void FStalkerResourcesManager::Destroy(UStalkerKinematicsComponent* Mesh)
{
	Meshes.Remove(Mesh);
	Mesh->MarkAsGarbage();
}


void FStalkerResourcesManager::RegisterKinematics(class UStalkerKinematicsComponent* Mesh)
{
	Mesh->Rename(nullptr,GetTransientPackage());
	Meshes.Add(Mesh);
}

void FStalkerResourcesManager::UnregisterKinematics(class UStalkerKinematicsComponent* Mesh)
{
	Meshes.Remove(Mesh);
}

void FStalkerResourcesManager::Refresh()
{
	GameSpawn = nullptr;
}

FString FStalkerResourcesManager::GetGamePath()
{
	switch (xrGameManager::GetGame())
	{
	case EGame::CS:
		return TEXT("/Game/CS");
	case EGame::SHOC:
		return TEXT("/Game/SHOC");
	default:
		return TEXT("/Game/COP");
	}
}

UStalkerGameSpawn* FStalkerResourcesManager::GetGameSpawn()
{
	if (!GameSpawn)
	{
		const FString ParentPackageName = GetGamePath() /TEXT("Spawns") / TEXT("GlobalSpawn");
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		GameSpawn = LoadObject<UStalkerGameSpawn>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
;	return GameSpawn;
}

#if WITH_EDITORONLY_DATA
UStalkerGameSpawn* FStalkerResourcesManager::GetOrCreateGameSpawn()
{
	GetGameSpawn();
	auto CreatePackageSpawn = [this]()
	{
		FString PackageName = GetGamePath() /TEXT("Spawns") / TEXT("GlobalSpawn");
		UPackage* BuiltDataPackage = CreatePackage(*PackageName);
		return BuiltDataPackage;
	};

	if (!GameSpawn || !GameSpawn->HasAllFlags(RF_Public | RF_Standalone))
	{
		if (GameSpawn)
		{
			GameSpawn->ClearFlags(RF_Standalone);
		}

		UPackage* BuiltDataPackage = CreatePackageSpawn();

		FName ShortPackageName = FPackageName::GetShortFName(BuiltDataPackage->GetFName());
		GameSpawn = NewObject<UStalkerGameSpawn>(BuiltDataPackage, ShortPackageName, RF_Standalone | RF_Public);
	}
	return GameSpawn;
}


#endif
void FStalkerResourcesManager::AddReferencedObjects(FReferenceCollector& Collector)
{
	for (auto& [Key, Value] : Brushes)
	{
		Collector.AddReferencedObject(Value->Brush);
	}
	for (auto& [Key, Value] : ShaderMaterials) {
		for (auto &[Key2, Value2] : Value) {
			Collector.AddReferencedObject(Value2->Material);
			Collector.AddReferencedObject(Value2->Texture);
		}
	}
	for (auto& [Key, Value] : Fonts)
	{
		Collector.AddReferencedObject(Value);
	}
	for (auto&  Value : Meshes)
	{
		Collector.AddReferencedObject(Value);
	}
	Collector.AddReferencedObject(GameSpawn);
}

FString FStalkerResourcesManager::GetReferencerName() const
{
	return TEXT("Stalker Resources Manager");
}

FStalkerResourcesManager::FStalkerResourcesManager()
{

}

FStalkerResourcesManager::~FStalkerResourcesManager()
{

}


