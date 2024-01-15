#include "Resources/StalkerResourcesManager.h"
#include "../Kernel/StalkerEngineManager.h"
#include "../Entities/Kinematics/StalkerKinematicsComponent.h"
#include "SkeletonMesh/StalkerKinematicsAssetUserData.h"
#include "../Entities/Levels/Light/StalkerLight.h"
#include "../Entities/Particles/StalkerNiagaraActor.h"
#include "Sound/StalkerSoundManager.h"
#include "Spawn/StalkerGameSpawn.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/xr_object.h"
THIRD_PARTY_INCLUDES_END


USlateBrushAsset* FStalkerResourcesManager::GetBrush(FName InNameMaterial, FName InNameTexture)
{
	bool NeedReload = false;
	check(InNameMaterial!=NAME_None);
	if (Brushes.Find(InNameMaterial))
	{
		if (Brushes[InNameMaterial].Find(InNameTexture))
		{

			
			USlateBrushAsset*  Result = Brushes[InNameMaterial][InNameTexture];
#if WITH_EDITORONLY_DATA
			if (BrushesNeedReloading.Find(Result))
			{
				NeedReload = true;
			}
			else
#endif
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
		UMaterialInterface* UnknownMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Base/Materials/UIUnkown.UIUnkown"));
		check(IsValid(UnknownMaterial));
		ParentMaterial = UnknownMaterial;
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
			UTexture* UnknownTexture = LoadObject<UTexture>(nullptr, TEXT("/Game/Base/Textures/Unkown.Unkown"));
			check(IsValid(UnknownTexture));
			Texture = UnknownTexture;
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
		USlateBrushAsset* NewSlateBrushAsset = Brushes[InNameMaterial][InNameTexture];
		NewSlateBrushAsset->SetFlags(RF_Transient);
		FVector2D TextureSize = FVector2D(32, 32);
		NewSlateBrushAsset->Brush = static_cast<FSlateBrush>(FSlateMaterialBrush(*Material, TextureSize));
		BrushesMaterials.Add(NewSlateBrushAsset, Material);
		if (InNameTexture != NAME_None)
		{
			BrushesTextures.Add(NewSlateBrushAsset, Texture);
		}
#if WITH_EDITORONLY_DATA
		BrushesNeedReloading.Remove(NewSlateBrushAsset);
#endif
		BrushesCounter[NewSlateBrushAsset]++;
		return NewSlateBrushAsset;
	}
	USlateBrushAsset* NewSlateBrushAsset = NewObject<USlateBrushAsset>();
	NewSlateBrushAsset->SetFlags(RF_Transient);
	FVector2D TextureSize = FVector2D(32,32);
	NewSlateBrushAsset->Brush =  static_cast<FSlateBrush>(FSlateMaterialBrush(*Material, TextureSize));
	Brushes.FindOrAdd(InNameMaterial).Add(InNameTexture, NewSlateBrushAsset);
	BrushesCounter.Add(NewSlateBrushAsset,1);
	BrushesMaterials.Add(NewSlateBrushAsset, Material);
	if (InNameTexture != NAME_None)
	{
		BrushesTextures.Add(NewSlateBrushAsset, Texture);
	}
	BrushInfo Info ={ InNameMaterial ,InNameTexture };
	BrushesInfo.Add(NewSlateBrushAsset, Info);
	return NewSlateBrushAsset;
}

UFont* FStalkerResourcesManager::GetFont(FName Name)
{
	if (Fonts.Contains(Name))
	{
		return Fonts[Name];
	}
	const FString DefaultFontPath = TEXT("/Engine/EngineFonts/Roboto.Roboto");
	FString FontPath = DefaultFontPath;
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

void FStalkerResourcesManager::Free(const USlateBrushAsset* Brush)
{
	int32*Counter = BrushesCounter.Find(Brush);
	check(Counter);
	if (--(*Counter) == 0)
	{
		UE_LOG(LogStalker, Log, TEXT("Destroy slate brush:[%s]%s"), *BrushesInfo[Brush].Matrrial.ToString(), *BrushesInfo[Brush].Texture.ToString());
		BrushesCounter.Remove(Brush);
#if WITH_EDITORONLY_DATA
		BrushesNeedReloading.Remove(Brush);
#endif
		BrushesMaterials.Remove(Brush);
		BrushesTextures.Remove(Brush);
		Brushes[BrushesInfo[Brush].Matrrial].Remove(BrushesInfo[Brush].Texture);
		if (Brushes[BrushesInfo[Brush].Matrrial].Num() == 0)
		{
			Brushes.Remove(BrushesInfo[Brush].Matrrial);
		}
		BrushesInfo.Remove(Brush);
	}
}

USlateBrushAsset* FStalkerResourcesManager::Copy(USlateBrushAsset* Brush)
{
	int32* Counter = BrushesCounter.Find(Brush);
	check(Counter);
	(*Counter)++;
	return Brush;
}

void FStalkerResourcesManager::CheckLeak() const
{
	check(Meshes.Num() == 0);
}

void FStalkerResourcesManager::Reload()
{
#if WITH_EDITORONLY_DATA
	for (auto& [Key, Data] : BrushesCounter)
	{
		BrushesNeedReloading.FindOrAdd(Key);
	}
#endif
}


class AStalkerLight* FStalkerResourcesManager::CreateLight()
{
#if WITH_EDITORONLY_DATA
	check(GWorld&&GWorld->IsGameWorld());
#endif
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.ObjectFlags = EObjectFlags::RF_Transient;
	AStalkerLight* Result = GWorld->SpawnActor< AStalkerLight>(SpawnParameters);
	Result->Lock();
	return Result;
}

void FStalkerResourcesManager::Destroy(class IRender_Light* InLight)
{
	AStalkerLight*Light =  static_cast<AStalkerLight*>(InLight);
	Light->Unlock();
	Light->Destroy();
}

void FStalkerResourcesManager::Destroy(class IParticleCustom* InParticles)
{
	AStalkerNiagaraActor*Particles =  static_cast<AStalkerNiagaraActor*>(InParticles);
	Particles->Destroy();
}

USkeletalMesh* FStalkerResourcesManager::GetKinematics(const char* InName)
{
	if (!FApp::IsGame()&&(!InName || !InName[0]))
	{
		return nullptr;
	}

	USkeletalMesh* Kinematics  = LoadObject<USkeletalMesh>(nullptr, *FString(InName), nullptr, LOAD_NoWarn);;

	FString Name = InName;
	if(!IsValid(Kinematics))
	{
	
		Name.ReplaceInline(TEXT("\\"), TEXT("/"));
		Name.ReplaceInline(TEXT("#"), TEXT("_"));
		switch (GStalkerEngineManager->GetCurrentGame())
		{
		default:
		{
			const FString ParentPackageName = TEXT("/Game/COP/Meshes") / Name;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Kinematics = LoadObject<USkeletalMesh>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
			if (!IsValid(Kinematics))
			{
				if (Name.StartsWith(TEXT("meshes/")))
				{
					FString NewName = Name;
					NewName.RemoveFromStart(TEXT("meshes/"));
					const FString ParentPackageNameFromLevel = TEXT("/Game/COP/Meshes/levels") / GStalkerEngineManager->GetCurrentWorldName() / NewName;
					const FString ParentObjectPathFromLevel = ParentPackageNameFromLevel + TEXT(".") + FPaths::GetBaseFilename(ParentPackageNameFromLevel);
					Kinematics = LoadObject<USkeletalMesh>(nullptr, *ParentObjectPathFromLevel, nullptr, LOAD_NoWarn);
				}
			}
		}
		break;
		case EStalkerGame::CS:
		{
			const FString ParentPackageName = TEXT("/Game/CS/Meshes") / Name;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Kinematics = LoadObject<USkeletalMesh>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
			if (!IsValid(Kinematics))
			{
				if (Name.StartsWith(TEXT("meshes/")))
				{
					FString NewName = Name;
					NewName.RemoveFromStart(TEXT("meshes/"));
					const FString ParentPackageNameFromLevel = TEXT("/Game/CS/Meshes/levels") / GStalkerEngineManager->GetCurrentWorldName() / NewName;
					const FString ParentObjectPathFromLevel = ParentPackageNameFromLevel + TEXT(".") + FPaths::GetBaseFilename(ParentPackageNameFromLevel);
					Kinematics = LoadObject<USkeletalMesh>(nullptr, *ParentObjectPathFromLevel, nullptr, LOAD_NoWarn);
				}
			}
		}
		break;
		case EStalkerGame::SHOC:
		{
			const FString ParentPackageName = TEXT("/Game/SHOC/Meshes") / Name;
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			Kinematics = LoadObject<USkeletalMesh>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
			if (!IsValid(Kinematics))
			{
				if (Name.StartsWith(TEXT("meshes/")))
				{
					FString NewName = Name;
					NewName.RemoveFromStart(TEXT("meshes/"));
					const FString ParentPackageNameFromLevel = TEXT("/Game/SHOC/Meshes/levels") / GStalkerEngineManager->GetCurrentWorldName() / NewName;
					const FString ParentObjectPathFromLevel = ParentPackageNameFromLevel + TEXT(".") + FPaths::GetBaseFilename(ParentPackageNameFromLevel);
					Kinematics = LoadObject<USkeletalMesh>(nullptr, *ParentObjectPathFromLevel, nullptr, LOAD_NoWarn);
				}
			}
		}
		break;
		}
	}
	if (!IsValid(Kinematics))
	{
		const FString ParentPackageName = TEXT("/Game/Base/Meshes") / Name;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		Kinematics = LoadObject<USkeletalMesh>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	if (!ensure(IsValid(Kinematics))||!ensure(IsValid(Kinematics->GetAssetUserData<UStalkerKinematicsAssetUserData>())))
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
		Kinematics = LoadObject<USkeletalMesh>(nullptr,TEXT("/Game/Base/Meshes/Error_KinematicsData.Error_KinematicsData"));
		check(Kinematics);
		check(Kinematics->GetAssetUserData<UStalkerKinematicsAssetUserData>());
	}
	return Kinematics;
}

class UStalkerKinematicsComponent* FStalkerResourcesManager::CreateKinematics(class USkeletalMesh* KinematicsData)
{
	UStalkerKinematicsComponent* Result =  NewObject< UStalkerKinematicsComponent>();
	Result->SetFlags(EObjectFlags::RF_Transient);
	Result->Initilize(KinematicsData);
	return Result;
}

class UStalkerKinematicsComponent* FStalkerResourcesManager::CreateKinematics(const char* InName, bool NeedReference)
{
	USkeletalMesh* Kinematics = GetKinematics(InName);
	if (IsValid(Kinematics))
	{
		UStalkerKinematicsComponent* Result = CreateKinematics(Kinematics);
		if (NeedReference)
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

void FStalkerResourcesManager::UnregisterKinematics(const class UStalkerKinematicsComponent* Mesh)
{
	Meshes.Remove(Mesh);
}

AStalkerNiagaraActor* FStalkerResourcesManager::CreateParticles(const char* Name)
{
#if WITH_EDITORONLY_DATA
	check(GWorld&&GWorld->IsGameWorld());
#endif
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.ObjectFlags = EObjectFlags::RF_Transient;
	AStalkerNiagaraActor* Result = GWorld->SpawnActor< AStalkerNiagaraActor>(SpawnParameters);
	Result->Initialize(Name);
	return Result;
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
		const FString PackageName = GetGamePath() /TEXT("Spawns") / TEXT("GlobalSpawn");
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

		const FName ShortPackageName = FPackageName::GetShortFName(BuiltDataPackage->GetFName());
		GameSpawn = NewObject<UStalkerGameSpawn>(BuiltDataPackage, ShortPackageName, RF_Standalone | RF_Public);
	}
	return GameSpawn;
}


#endif
void FStalkerResourcesManager::AddReferencedObjects(FReferenceCollector& Collector)
{
	for (auto& [Key, Value] : BrushesMaterials)
	{
		Collector.AddReferencedObject(Key);
		Collector.AddReferencedObject(Value);
	}
	for (auto& [Key, Value] : BrushesTextures)
	{
		Collector.AddReferencedObject(Value);
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
	Collector.AddReferencedObject(SoundManager);
}

FString FStalkerResourcesManager::GetReferencerName() const
{
	return TEXT("Stalker Resources Manager");
}

FStalkerResourcesManager::FStalkerResourcesManager()
{
	SoundManager = NewObject<UStalkerSoundManager>();
}

FStalkerResourcesManager::~FStalkerResourcesManager()
{
}


