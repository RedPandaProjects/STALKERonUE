#include "StalkerEngineManager.h"

#include "MoviePlayer.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Resources/StalkerResourcesManager.h"
#include "Resources/Sound/StalkerSoundManager.h"
#include "Kernel/Unreal/UI/LoadingScreen/StalkerBaseLoadingScreen.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XrDeviceInterface.h"
THIRD_PARTY_INCLUDES_END
#include "Unreal/GameViewportClient/StalkerGameViewportClient.h"
#include "RBMK/Core/RBMKMemory.h"
#include "RBMK/Core/RBMKDebug.h"
#include "RBMK/Core/RBMKLog.h"
#include "RBMK/Engine/RBMKInput.h"
#include "RBMK/Engine/RBMKEngine.h"
#include "Unreal/GameSettings/StalkerGameSettings.h"
#include "GameDelegates.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterialsManager.h"
#if WITH_EDITORONLY_DATA
#include "Editor.h"
#endif
#include "Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/Spawn/StalkerGameSpawn.h"
#include "Resources/CFrom/StalkerCForm.h"
#include "Resources/AIMap/StalkerAIMap.h"
#include "Resources/Spawn/StalkerLevelSpawn.h"
#include "NiagaraTypes.h"
#include "../Resources/Particle/StalkerParticleDomain.h"

STALKER_API FStalkerEngineManager* GStalkerEngineManager = nullptr;
static FRBMKMemory	GXRayMemory;
static FRBMKLog	GXRayLog;
static FRBMKDebug	GXRayDebug;

FStalkerEngineManager::FStalkerEngineManager()
{
	
}

FStalkerEngineManager::~FStalkerEngineManager()
{
	if (!GIsEditor)
	{
		AppEnd();
	}
	FCoreDelegates::OnPostEngineInit.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	if(IsValid(GEngine))
	{
		GEngine->OnTravelFailure().RemoveAll(this);
	}
#if WITH_EDITOR
	FGameDelegates::Get().GetEndPlayMapDelegate().RemoveAll(this);
#endif
#if WITH_EDITORONLY_DATA
	FCoreDelegates::OnGetOnScreenMessages.RemoveAll(this);
#endif
	DetachViewport(GameViewportClient);
	g_Engine->Destroy();
	delete g_Engine;
	MyXRayEngine = nullptr;
	Core.Destroy();
	GameMaterialLibrary = nullptr;
	ResourcesManager->CheckLeak();
	delete ResourcesManager;
}

void FStalkerEngineManager::Initialize()
{
	FString GameShaderDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Game"), GameShaderDir);

	ResourcesManager = new FStalkerResourcesManager;
	PhysicalMaterialsManager = NewObject<UStalkerPhysicalMaterialsManager>();
	MyXRayInput = nullptr;
	FString FSName;
	EGamePath GamePath = EGamePath::COP_1602;
	if (GIsEditor)
	{
		FSName = TEXT("fs");
		switch (GetDefault<UStalkerGameSettings>()->EditorStartupGame)
		{
		case EStalkerGame::CS:
			GamePath = EGamePath::CS_1510;
			FSName += TEXT("_cs");
			break;
		case EStalkerGame::SHOC:
			GamePath = EGamePath::SHOC_10006;
			FSName += TEXT("_soc");
			break;
		}
		CurrentGame = GetDefault<UStalkerGameSettings>()->EditorStartupGame;
	}
	else
	{
		CurrentGame = EStalkerGame::SHOC;
		GamePath = EGamePath::SHOC_10006;
		FSName = TEXT("fsgame_soc");
	}

	FSName += TEXT(".ltx");
	FSName = FPaths::Combine(FPaths::ProjectDir(), FSName);
	Core.Initialize(&GXRayMemory, &GXRayLog, &GXRayDebug, TCHAR_TO_ANSI(*FSName), GIsEditor, GamePath);

	MyXRayEngine = new FRBMKEngine;
	g_Engine = MyXRayEngine;
	GameMaterialLibrary = PhysicalMaterialsManager;
	g_Engine->Initialize();
#if WITH_EDITOR
	FGameDelegates::Get().GetEndPlayMapDelegate().AddRaw(this, &FStalkerEngineManager::OnEndPlayMap);
#endif
#if WITH_EDITORONLY_DATA
	FCoreDelegates::OnGetOnScreenMessages.AddRaw(this, &FStalkerEngineManager::OnGetOnScreenMessages);
#endif
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddRaw(this, &FStalkerEngineManager::OnPostLoadMap);
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FStalkerEngineManager::OnPostEngineInit);
#if !WITH_EDITOR
	AppStart();
#endif
	
}

void FStalkerEngineManager::AppStart()
{
	PhysicalMaterialsManager->Build();
	GetResourcesManager()->GetSoundManager()->Build();
	Device->seqAppStart.Process(rp_AppStart);
	Device->b_is_Active = TRUE;
}

void FStalkerEngineManager::AppEnd()
{
	Device->seqParallel.clear();
	Device->b_is_Active = FALSE;
	Device->seqAppEnd.Process(rp_AppEnd);
	MyXRayInput->ClearStates();
	PhysicalMaterialsManager->Clear();
	GetResourcesManager()->GetSoundManager()->Clear();
}

void FStalkerEngineManager::AddReferencedObjects(FReferenceCollector& Collector)
{
	if(PhysicalMaterialsManager)
	{
		Collector.AddReferencedObject(PhysicalMaterialsManager);
	}
	if(CurrentAIMap)
	{
		Collector.AddReferencedObject(CurrentAIMap);
	}
}

FString FStalkerEngineManager::GetReferencerName() const
{
	return TEXT("StalkerEngineManager");
}

void FStalkerEngineManager::AttachViewport(class UGameViewportClient* InGameViewportClient)
{
	if (GameViewportClient )
	{
		DetachViewport(GameViewportClient);
	}
	if (InGameViewportClient == nullptr)
	{
		return;
	}
	GameViewportClient = CastChecked<UStalkerGameViewportClient>(InGameViewportClient);
	FViewport::ViewportResizedEvent.AddRaw(this, &FStalkerEngineManager::OnViewportResized);
	FVector2D ScreenSize;
	GameViewportClient->GetViewportSize(ScreenSize);
	if (u32(ScreenSize.X) != Device->dwWidth || u32(ScreenSize.Y) != Device->dwHeight)
	{
		Device->dwWidth = u32(ScreenSize.X);
		Device->dwHeight = u32(ScreenSize.Y);
		Device->fASPECT = ScreenSize.Y / ScreenSize.X;
		Device->fWidth_2 = ScreenSize.X / 2;
		Device->fHeight_2 = ScreenSize.Y / 2;
		Device->seqDeviceReset.Process(rp_DeviceReset);
		Device->seqResolutionChanged.Process(rp_ScreenResolutionChanged);
	}
	GameViewportClient->OnCloseRequested().AddRaw(this, &FStalkerEngineManager::OnViewportCloseRequested);
	if (GameViewportClient->IsActive)
	{
		Device->seqAppActivate.Process(rp_AppActivate);
	}
}

void FStalkerEngineManager::DetachViewport(class UGameViewportClient* InGameViewportClient)
{
	if (GameViewportClient&&GameViewportClient == InGameViewportClient)
	{
		FViewport::ViewportResizedEvent.RemoveAll(this);
		GameViewportClient->OnCloseRequested().RemoveAll(this);
		GameViewportClient = nullptr	;
	
	}
}

FString FStalkerEngineManager::GetCurrentWorldName()
{
	return CurrentWorldName;
}

FString FStalkerEngineManager::GetGamePath()
{
	switch (GStalkerEngineManager->GetCurrentGame())
	{
	case EStalkerGame::CS:
		return TEXT("/Game/CS");
	case EStalkerGame::SHOC:
		return TEXT("/Game/SHOC");
	default:
		return TEXT("/Game/COP");
	}
}

class UStalkerAIMap* FStalkerEngineManager::GetLevelGraph(FString LevelName)
{
	UStalkerGameSpawn* GameSpawn = GetResourcesManager()->GetGameSpawn();
	check (IsValid(GameSpawn) && GameSpawn->GameSpawnGuid.IsValid());
	FStalkerGameSpawnLevelInfo* LevelInfo = GameSpawn->LevelsInfo.FindByPredicate([&LevelName](const FStalkerGameSpawnLevelInfo& LevelInfo)
	{
		return LevelInfo.Name == LevelName;
	});
	check(LevelInfo);
	const FString ParentPackageName = FPaths::GetPath(LevelInfo->Map.ToString()) / FPaths::GetBaseFilename(LevelInfo->Map.ToString()) + TEXT("_AIMap");
	const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
	CurrentAIMap = LoadObject<UStalkerAIMap>(nullptr, *ParentObjectPath, nullptr);
	check(CurrentAIMap);
	return CurrentAIMap;
}

void FStalkerEngineManager::ReInitialized(EStalkerGame Game)
{
#if WITH_EDITOR
	if (GEditor->PlayWorld != nullptr||GEditor->bIsSimulatingInEditor)
	{
		return ;
	}
#endif
	if (FApp::IsGame())
	{
		return;
	}
	if (GetCurrentGame() == Game)
	{
		return;
	}

	PreReInitializedMulticastDelegate.Broadcast();

	g_Engine->Destroy();
	MyXRayEngine = nullptr;
	delete g_Engine;
	Core.Destroy();
	FString FSName;
	EGamePath GamePath = EGamePath::COP_1602;

	if (GIsEditor)
	{
		FSName = TEXT("fs");
	}
	else
	{
		FSName = TEXT("fsgame");
	}
	CurrentGame = Game;
	switch (Game)
	{
	case EStalkerGame::CS:
		GamePath = EGamePath::CS_1510;
		FSName += TEXT("_cs");
		break;
	case EStalkerGame::SHOC:
		GamePath = EGamePath::SHOC_10006;
		FSName += TEXT("_soc");
		break;
	}

	FSName += TEXT(".ltx");
	FSName = FPaths::Combine(FPaths::ProjectDir(), FSName);

	Core.Initialize(&GXRayMemory, &GXRayLog, &GXRayDebug, TCHAR_TO_ANSI(*FSName), GIsEditor, GamePath);
	MyXRayEngine = new FRBMKEngine;
	g_Engine = MyXRayEngine;
	g_Engine->Initialize();

	PostReInitializedMulticastDelegate.Broadcast();
}

void FStalkerEngineManager::SetInput(class FRBMKInput* InXRayInput)
{
	if (InXRayInput == nullptr) 
	{ 
		MyXRayInput = nullptr;
		return;
	}
	check(!MyXRayInput);
	MyXRayInput = InXRayInput;
}

void FStalkerEngineManager::LoadDefaultWorld()
{
	const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
	if(IsValid(GWorld))
	{
		CurrentWorldPath = 	UWorld::RemovePIEPrefix(*GWorld->GetPathName());
		if (GameMapsSettings->GetGameDefaultMap() == CurrentWorldPath.GetLongPackageName())
		{
			return;
		}
	}
	const ETravelType TravelType = TRAVEL_Absolute;
	FWorldContext& WorldContext = GEngine->GetWorldContextFromWorldChecked(GWorld);
	FString Cmd = GameMapsSettings->GetGameDefaultMap();
	FURL TestURL(&WorldContext.LastURL, *Cmd, TravelType);
	if (TestURL.IsLocalInternal())
	{
		check (GEngine->MakeSureMapNameIsValid(TestURL.Map));
	}
	GEngine->SetClientTravel(GWorld, *Cmd, TravelType);
	WorldStatus = EStalkerWorldStatus::None;
	CurrentWorldPath.Reset();
	CurrentWorldName.Empty();
}

bool FStalkerEngineManager::LoadWorld(FString LevelName)
{
	GetResourcesManager()->GetSoundManager()->Clear();
	CurrentWorldName.Empty();
	CurrentWorldPath.Reset();
	UWorld* CurrentWorld = GWorld;
	UStalkerGameSpawn* GameSpawn = GetResourcesManager()->GetGameSpawn();
	if (!IsValid(GameSpawn) || !GameSpawn->GameSpawnGuid.IsValid())
	{
		WorldStatus = EStalkerWorldStatus::Failure;
		return false;
	}
	FStalkerGameSpawnLevelInfo* LevelInfo = GameSpawn->LevelsInfo.FindByPredicate([&LevelName](const FStalkerGameSpawnLevelInfo& LevelInfo)
	{
		return LevelInfo.Name == LevelName;
	});
	if (!LevelInfo)
	{
		WorldStatus = EStalkerWorldStatus::Failure;
		return false;
	}
    CurrentWorldPath = 	UWorld::RemovePIEPrefix(*CurrentWorld->GetPathName());
	if (LevelInfo->Map == CurrentWorldPath)
	{
		WorldStatus = EStalkerWorldStatus::Ready;
		if (!CheckCurrentWorld())
		{
			CurrentWorldName.Empty();
			CurrentWorldPath.Reset();
			WorldStatus = EStalkerWorldStatus::Failure;
			return false;
		}
		return true;
	}
	const ETravelType TravelType = TRAVEL_Absolute;
	FWorldContext& WorldContext = GEngine->GetWorldContextFromWorldChecked(CurrentWorld);
	FString Cmd = LevelInfo->Map.GetAssetName();
	FURL TestURL(&WorldContext.LastURL, *Cmd, TravelType);
	if (TestURL.IsLocalInternal())
	{
		if (!GEngine->MakeSureMapNameIsValid(TestURL.Map))
		{
			UE_LOG(LogStalker, Error, TEXT("The map '%s' does not exist."), *TestURL.Map);
			return false;
		}
	}
	WorldStatus = EStalkerWorldStatus::Loading;
	CurrentWorldPath = LevelInfo->Map;
	CurrentWorldName = LevelInfo->Name;
	GEngine->SetClientTravel(CurrentWorld, *Cmd, TravelType);
	return true;
}

bool FStalkerEngineManager::CheckCurrentWorld()
{
	UWorld* World = GWorld;
	if (!IsValid(World))
	{
		return false;
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!IsValid(StalkerWorldSettings))
	{
		return false;
	}
	UStalkerCForm* CForm = StalkerWorldSettings->GetCForm();
	if (!IsValid(CForm) || CForm->Triangles.IsEmpty())
	{
		return false;
	}

	UStalkerGameSpawn* GameSpawn = GetResourcesManager()->GetGameSpawn();
	if (!IsValid(GameSpawn) || !GameSpawn->GameSpawnGuid.IsValid())
	{
		return false;
	}
	UStalkerAIMap* AIMap = StalkerWorldSettings->GetAIMap();
	if (!IsValid(AIMap) || !AIMap->AIMapGuid.IsValid())
	{
		return false;
	}
	FSoftObjectPath CurrentInWorldPath = UWorld::RemovePIEPrefix(*World->GetPathName());
	FStalkerGameSpawnLevelInfo* LevelInfo = GameSpawn->LevelsInfo.FindByPredicate([&CurrentInWorldPath](const FStalkerGameSpawnLevelInfo& LevelInfo)
	{
		return LevelInfo.Map == CurrentInWorldPath;
	});
	if (!LevelInfo)
	{
		return false;
	}
	if (LevelInfo->AIMapGuid != AIMap->AIMapGuid)
	{
		return false;
	}
	return true;
}

void FStalkerEngineManager::OnViewportResized(FViewport* InViewport, uint32)

{
	if (GameViewportClient && GameViewportClient->Viewport == InViewport)
	{
		FVector2D ScreenSize;
		GameViewportClient->GetViewportSize(ScreenSize);
		if (u32(ScreenSize.X) != Device->dwWidth || u32(ScreenSize.Y) != Device->dwHeight)
		{
			Device->fASPECT = ScreenSize.Y / ScreenSize.X;
			Device->dwWidth = u32(ScreenSize.X);
			Device->dwHeight = u32(ScreenSize.Y);
			Device->fWidth_2 = ScreenSize.X / 2;
			Device->fHeight_2 = ScreenSize.Y / 2;
			Device->seqDeviceReset.Process(rp_DeviceReset);
			Device->seqResolutionChanged.Process(rp_ScreenResolutionChanged);
		}
	}
}

void FStalkerEngineManager::OnEndPlayMap()
{
#if WITH_EDITOR
	ReInitialized(GetDefault<UStalkerGameSettings>()->EditorStartupGame);
#endif

}

void FStalkerEngineManager::SetupLoadingScreen()
{
	if (!IsRunningDedicatedServer())
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.WidgetLoadingScreen = SNew(SStalkerBaseLoadingScreen);
		LoadingScreen.bAllowEngineTick = true;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}
}
#if WITH_EDITORONLY_DATA
void FStalkerEngineManager::OnGetOnScreenMessages(FCoreDelegates::FSeverityMessageMap& Out)
{
	UWorld* World = GWorld;
	if (!IsValid(World))
	{
		return;
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (IsValid(StalkerWorldSettings)&&!StalkerWorldSettings->NotForXRay)
	{
		UStalkerAIMap* AIMap = StalkerWorldSettings->GetAIMap();
		if (!IsValid(AIMap) || AIMap->NeedRebuild)
		{
			Out.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, FText::FromString(TEXT("NEED REBUILD AI MAP!")));
		}
		UStalkerLevelSpawn* Spawn = StalkerWorldSettings->GetSpawn();
		if (!IsValid(Spawn) || Spawn->NeedRebuild||StalkerWorldSettings->NeedRebuildSpawn)
		{
			Out.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, FText::FromString(TEXT("NEED REBUILD LEVEL SPAWN!")));
		}
	}

	if (!FApp::IsGame())
	{
		return;
	}
	if (!IsValid(StalkerWorldSettings))
	{
		Out.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, FText::FromString(TEXT("WORLD INVALID!")));
		Out.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, FText::FromString(TEXT("* NEED AStalkerWorldSettings!")));
		return;
	}
	if(StalkerWorldSettings->NotForXRay)
	{
		return;
	}
	UStalkerCForm* CForm = StalkerWorldSettings->GetCForm();
	if (!IsValid(CForm) || CForm->Triangles.IsEmpty())
	{
		Out.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, FText::FromString(TEXT("WORLD INVALID!")));
		Out.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, FText::FromString(TEXT("* CFORM IS EMPTY!")));
		return;
	}

	UStalkerGameSpawn*	GameSpawn =  GetResourcesManager()->GetGameSpawn();
	if (!IsValid(GameSpawn) || !GameSpawn->GameSpawnGuid.IsValid())
	{
		Out.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, FText::FromString(TEXT("WORLD INVALID!")));
		Out.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, FText::FromString(TEXT("* GAME SPAWN IS EMPTY,SEE LOG!")));
		return;
	}
	UStalkerAIMap* AIMap = StalkerWorldSettings->GetAIMap();
	if (!IsValid(AIMap) || !AIMap->AIMapGuid.IsValid())
	{
		Out.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, FText::FromString(TEXT("WORLD INVALID!")));
		Out.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, FText::FromString(TEXT("* AI MAP IS EMPTY,SEE LOG!")));
		return;
	}
	FSoftObjectPath CurrentInWorldPath = UWorld::RemovePIEPrefix(*World->GetPathName());
	FStalkerGameSpawnLevelInfo* LevelInfo = GameSpawn->LevelsInfo.FindByPredicate([&CurrentInWorldPath](const FStalkerGameSpawnLevelInfo&LevelInfo)
	{
		return LevelInfo.Map == CurrentInWorldPath;
	});
	if (!LevelInfo)
	{
		Out.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, FText::FromString(TEXT("WORLD INVALID!")));
		Out.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, FText::FromString(TEXT("* CURRENT WORLD NOT FOUND IN GAME SPAWN!")));
		return;
	}
	if (LevelInfo->AIMapGuid!= AIMap->AIMapGuid)
	{
		Out.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, FText::FromString(TEXT("WORLD INVALID!")));
		Out.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, FText::FromString(TEXT("* GAME SPAWN INVALID BECOUSE AI MAP IS ALREADY DIFFERENT!")));
		return;
	}

}
#endif

void FStalkerEngineManager::OnPostEngineInit()
{
	GEngine->OnTravelFailure().AddRaw(this, &FStalkerEngineManager::OnTravelFailure);

	FNiagaraTypeRegistry::Register(FStalkerParticleDomain::StaticStruct(), 	ENiagaraTypeRegistryFlags::AllowAnyVariable |ENiagaraTypeRegistryFlags::AllowParameter|ENiagaraTypeRegistryFlags::AllowPayload);
	FNiagaraTypeRegistry::Register(StaticEnum<EStalkerParticleDomainType>(), ENiagaraTypeRegistryFlags::AllowAnyVariable | ENiagaraTypeRegistryFlags::AllowParameter);
	FNiagaraTypeRegistry::Register(FNiagaraTypeDefinition(StaticEnum<EStalkerParticleDomainType>()).ToStaticDef(), ENiagaraTypeRegistryFlags::AllowAnyVariable | ENiagaraTypeRegistryFlags::AllowParameter);
#if WITH_EDITOR
	if (!GIsEditor)
	{
		AppStart();
	}
#endif
}

void FStalkerEngineManager::OnPostLoadMap(UWorld* World)
{
#if WITH_EDITOR
	if (GIsEditor)
	{
		const bool bIsInPIE = (GEditor->PlayWorld != nullptr) && (!GEditor->bIsSimulatingInEditor);
		if (!bIsInPIE)
		{
			return;
		}
	}
#endif
	if (WorldStatus == EStalkerWorldStatus::Loading)
	{
		FSoftObjectPath CurrentPostLoadWorldPath = UWorld::RemovePIEPrefix(*World->GetPathName());
		check(CurrentPostLoadWorldPath == CurrentWorldPath);
#if !UE_BUILD_SHIPPING
		if (!CheckCurrentWorld())
		{
			CurrentWorldName.Empty();
			CurrentWorldPath.Reset();
			WorldStatus = EStalkerWorldStatus::Failure;
			return;
		}
#else
		check(CheckCurrentWorld());
#endif
		WorldStatus = EStalkerWorldStatus::Ready;
	}
}

void FStalkerEngineManager::OnTravelFailure(UWorld* NextWorld, ETravelFailure::Type FailureType, const FString& Name)
{
	if (WorldStatus == EStalkerWorldStatus::Loading)
	{
		CurrentWorldName.Empty();
		CurrentWorldPath.Reset();
		WorldStatus = EStalkerWorldStatus::Failure;
	}
}

void FStalkerEngineManager::OnViewportCloseRequested(FViewport* InViewport)
{
	check(GameViewportClient->Viewport== InViewport);
	DetachViewport(GameViewportClient);
}
