#include "StalkerEditorManager.h"
#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"
#include "Managers/CFrom/StalkerEditorCForm.h"
#include "Managers/AIMap/StalkerEditorAIMap.h"
#include "UI/StalkerEditorStyle.h"
#include "Entities/EditorRender/StalkerEditorRenderProxy.h"
#include "PlacementMode/Public/IPlacementModeModule.h"
#include "Entities/Scene/SpawnObject/StalkerSpawnObject.h"
#include "Entities/Scene/SpawnObject/StalkerSpawnObjectFactory.h"
#include "Managers/SEFactory/StalkerSEFactoryManager.h"
#include "Managers/Spawn/StalkerEditorSpawn.h"
#include "Resources/Spawn/StalkerGameSpawn.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterialsManager.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"
#include "HAL/FileManager.h"
#include "Kernel/Unreal/WorldSettings/StalkerWorldSettings.h"
///////////////////////////////////////////////////////////////////////////////////////////
#include "Entities/Scene/SpawnObject/Properties/StalkerSpawnPropertiesTypes.h"
#include "Entities/Scene/SpawnObject/Properties/StalkerSpawnPropertiesTypeCustomization.h"
///////////////////////////////////////////////////////////////////////////////////////////
#include "UI/Commands/StalkerEditorCommands.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XRayEngineInterface.h"
#include "Importer/XRayEngineFactory.h"
THIRD_PARTY_INCLUDES_END

UStalkerEditorManager* GStalkerEditorManager = nullptr;
#define LOCTEXT_NAMESPACE "StalkerEditor"
void UStalkerEditorManager::Initialized()
{

	if (GIsEditor)
	{
		UICommandList = MakeShareable(new FUICommandList);
		GXRayObjectLibrary = new XRayObjectLibrary;
		GXRayObjectLibrary->OnCreate();
		if (GStalkerEngineManager->GetCurrentGame() == EStalkerGame::SHOC)
		{
			SOCMaterials.Load();
		}
		GStalkerEngineManager->PostReInitializedMulticastDelegate.AddUObject(this, &UStalkerEditorManager::OnReInitialized);

		ScanSkeletons();
		EditorCFrom = NewObject<UStalkerEditorCForm>(this);
		EditorCFrom->Initialize();
		EditorAIMap = NewObject<UStalkerEditorAIMap>(this);
		EditorAIMap->Initialize();
		SEFactoryManager = NewObject<UStalkerSEFactoryManager>(this);
		EditorSpawn = NewObject<UStalkerEditorSpawn>(this);
		EditorSpawn->Initialize();

		FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UStalkerEditorManager::OnPostWorldInitialization);

		{
			FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_Storys::StaticStruct()->GetFName(),	FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_StorysCustomization::MakeInstance));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_SpawnStorys::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_SpawnStorysCustomization::MakeInstance));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_Location1::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_LocationCustomization::MakeInstance, 0));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_Location2::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_LocationCustomization::MakeInstance, 1));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_Location3::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_LocationCustomization::MakeInstance, 2));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_Location4::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_LocationCustomization::MakeInstance, 3));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_LightAnim::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_LightAnimCustomization::MakeInstance));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_CharacterProfile::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_CharacterProfileCustomization::MakeInstance));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_SmartCoverDescription::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_SmartCoverDescriptionCustomization::MakeInstance));

		}
		FEditorDelegates::PreBeginPIE.AddUObject(this, &UStalkerEditorManager::OnPreBeginPIE);
		FEditorDelegates::PostPIEStarted.AddUObject(this, &UStalkerEditorManager::OnPostPIEStarted);
		FEditorDelegates::EndPIE.AddUObject(this, &UStalkerEditorManager::OnEndPIE);

		GStalkerEditorManager->UICommandList->MapAction(StalkerEditorCommands::Get().ReloadConfigsAndScript, FExecuteAction::CreateUObject(this, &UStalkerEditorManager::ReloadConfigs));
		GStalkerEditorManager->UICommandList->MapAction(StalkerEditorCommands::Get().ImportUITextures, FExecuteAction::CreateUObject(this, &UStalkerEditorManager::ImportUITextures));
		GStalkerEditorManager->UICommandList->MapAction(StalkerEditorCommands::Get().ImportMeshes, FExecuteAction::CreateUObject(this, &UStalkerEditorManager::ImportMeshes));
		GStalkerEditorManager->UICommandList->MapAction(StalkerEditorCommands::Get().ImportPhysicalMaterials, FExecuteAction::CreateUObject(this, &UStalkerEditorManager::ImportPhysicalMaterials));
	}
	
}

void UStalkerEditorManager::Destroy()
{
	if (GIsEditor)
	{
		FEditorDelegates::PreBeginPIE.RemoveAll(this);
		if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
		{
			FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_Storys::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_SpawnStorys::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_Location1::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_Location2::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_Location3::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_Location4::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_LightAnim::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_CharacterProfile::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_SmartCoverDescription::StaticStruct()->GetFName());
		}

		UICommandList.Reset();
		FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
		SEFactoryManager->UnLoad();
		SEFactoryManager->Destroy();
		SEFactoryManager->MarkAsGarbage();
		SEFactoryManager = nullptr;
		EditorAIMap->Destroy();
		EditorAIMap->MarkAsGarbage();
		EditorAIMap = nullptr;
		EditorCFrom->Destroy();
		EditorCFrom->MarkAsGarbage();
		EditorCFrom = nullptr;
		EditorSpawn->Destroy();
		EditorSpawn->MarkAsGarbage();
		EditorSpawn = nullptr;
		GStalkerEngineManager->PostReInitializedMulticastDelegate.RemoveAll(this);
		GXRayObjectLibrary->OnDestroy();
		delete GXRayObjectLibrary;
	}
}

FString UStalkerEditorManager::GetGamePath()
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

void UStalkerEditorManager::ReloadConfigs()
{
	if (FApp::IsGame())
	{
		return;
	}
	SEFactoryManager->UnLoad();
	delete pSettings;
	string_path 			si_name;
	FS.update_path(si_name, "$game_config$", "system.ltx");
	pSettings = xr_new<CInifile>(si_name, TRUE);// FALSE,TRUE,TRUE);
	delete pGameIni;
	string_path					fname;
	FS.update_path(fname, "$game_config$", "game.ltx");
	pGameIni = xr_new<CInifile>(fname, TRUE);
	SEFactoryManager->Load();
}

void UStalkerEditorManager::ImportUITextures()
{
	auto ImportFromPath = [](const char*PathName)
	{
		string_path	GameTexturePath;
		FS.update_path(GameTexturePath, "$game_textures$", PathName);
		TArray<FString> Files;
		FString TexturePath = GameTexturePath;
		TexturePath.ReplaceCharInline(TEXT('\\'),TEXT('/'));
		IFileManager::Get().FindFilesRecursive(Files,*TexturePath,TEXT("*.dds"),true,false);
	
		XRayEngineFactory EngineFactory(nullptr, RF_Standalone | RF_Public);
		for(FString FileName:Files)
		{
			FileName = FPaths::GetPath(FileName) / FPaths::GetBaseFilename(FileName);
			if(FCString::Strstr((*FileName) + TexturePath.Len() - FCStringAnsi::Strlen(PathName),TEXT("ui_font"))!=nullptr)
				continue;
			EngineFactory.ImportTexture((*FileName)+ TexturePath.Len() - FCStringAnsi::Strlen(PathName),true);
		}
	};
	ImportFromPath("ui");
	ImportFromPath("map");

}

void UStalkerEditorManager::ImportMeshes()
{
	string_path	GameMeshesPath;
	FS.update_path(GameMeshesPath, "$game_meshes$","");
	TArray<FString> Files;
	FString MeshesPath = GameMeshesPath;
	MeshesPath.ReplaceCharInline(TEXT('\\'),TEXT('/'));
	IFileManager::Get().FindFilesRecursive(Files,*MeshesPath,TEXT("*.ogf"),true,false);
	for(FString FileName:Files)
	{
		FString FileNameWithoutExtension = FPaths::GetPath(FileName) / FPaths::GetBaseFilename(FileName);
		FString UnrealPath = GStalkerEditorManager->GetGamePath() / TEXT("Meshes");
		UnrealPath = UnrealPath/ (*FileNameWithoutExtension + MeshesPath.Len());
		XRayEngineFactory EngineFactory(CreatePackage(*FPaths::GetPath(UnrealPath)), RF_Standalone | RF_Public);
		EngineFactory.ImportOGF(FileName);
	}
}

void UStalkerEditorManager::ImportPhysicalMaterials()
{
	string_path	GameMaterialFilePath;
	FS.update_path(GameMaterialFilePath, "$game_data$", "gamemtl.xr");
	XRayEngineFactory EngineFactory(nullptr, RF_Standalone | RF_Public);
	EngineFactory.ImportPhysicsMaterials(ANSI_TO_TCHAR(GameMaterialFilePath));
}

void UStalkerEditorManager::OnPreBeginPIE(const bool)
{
	const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>();
	
	if (FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport))
	{
		if (UWorld* World = WorldContext->World())
		{
			if (AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings()))
			{
				if (StalkerWorldSettings->NotForXRay)
				{
				
					GStalkerEngineManager->GetResourcesManager()->Reload();
					GStalkerEngineManager->AppStart();
					return;
				}
			}
		
		}
	}
	if (SGSettings->NeedAutoBuildCForm)
	{
		EditorCFrom->Build();
	}
	if (SGSettings->NeedAutoBuildAIMap)
	{
		EditorAIMap->BuildIfNeeded();
	}
	class UStalkerLevelSpawn* LevelSpawn = nullptr;
	if (SGSettings->NeedAutoBuildLevelSpawn)
	{
		LevelSpawn = EditorSpawn->BuildLevelSpawnIfNeeded();
	}
	if (SGSettings->NeedAutoBuildGameSpawn)
	{
		if (SGSettings->UseCurrentWorldSpawn)
		{
			if (LevelSpawn)
			{
				EditorSpawn->BuildGameSpawn(LevelSpawn, true);
			}
			else
			{
				UStalkerGameSpawn* GameSpawn = GStalkerEngineManager->GetResourcesManager()->GetOrCreateGameSpawn();
				check(GameSpawn);
				GameSpawn->InvalidGameSpawn();
			}
		}
		else
		{
			EditorSpawn->BuildGameSpawn(nullptr,true);
		}
	}
	GStalkerEngineManager->GetResourcesManager()->Reload();
	GStalkerEngineManager->AppStart();
}

void UStalkerEditorManager::OnPostPIEStarted(const bool)
{
	if (IsValid(GWorld))
	{
		if (AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(GWorld->GetWorldSettings()))
		{
			if (!StalkerWorldSettings->NotForXRay)
			{
				g_Engine->RunGame();
			}
		}
	}

}

void UStalkerEditorManager::OnEndPIE(const bool)
{
	g_Engine->StopGame();
	GStalkerEngineManager->AppEnd();
}

void UStalkerEditorManager::OnReInitialized()
{
	GXRayObjectLibrary->OnDestroy();
	GXRayObjectLibrary->OnCreate();
	SOCMaterials.UnLoad();
	if (GStalkerEngineManager->GetCurrentGame() == EStalkerGame::SHOC)
	{
		SOCMaterials.Load();
	}
}

void UStalkerEditorManager::ScanSkeletons()
{
	Skeletons.Empty();
	const FString PackageName = UPackageTools::SanitizePackageName(GetGamePath());
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByPath(FName(*PackageName), AssetData, true);
	for (FAssetData& Data : AssetData)
	{
		USkeleton* Skeleton = Cast<USkeleton>(Data.GetAsset());
		if (Skeleton)
		{
			Skeletons.Add(Skeleton);
		}
	}

}

void UStalkerEditorManager::OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues)
{
	for (TActorIterator<AStalkerEditorRenderProxy> AactorItr(World); AactorItr; ++AactorItr)
	{
		if (AactorItr)
		{
			return;
		}
	}
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.ObjectFlags = RF_Transient;
	World->SpawnActor< AStalkerEditorRenderProxy>(ActorSpawnParameters);
}




#undef LOCTEXT_NAMESPACE