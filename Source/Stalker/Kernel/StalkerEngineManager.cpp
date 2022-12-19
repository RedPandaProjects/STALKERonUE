#include "StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"
#include "XRay/Render/Resources/SkeletonMesh/XRaySkeletonMeshManager.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XrDeviceInterface.h"
#include "XrEngine/XRayEngineInterface.h"
THIRD_PARTY_INCLUDES_END
#include "Unreal/GameViewportClient/StalkerGameViewportClient.h"
#include "XRay/Core/XRayMemory.h"
#include "XRay/Core/XRayDebug.h"
#include "XRay/Core/XRayLog.h"
#include "XRay/Core/XRayInput.h"
#include "XRay/Core/XRayEngine.h"
#include "Unreal/GameSettings/StalkerGameSettings.h"
#include "GameDelegates.h"

STALKER_API UStalkerEngineManager* GXRayEngineManager = nullptr;

void UStalkerEngineManager::AttachViewport(class UGameViewportClient* InGameViewportClient)
{
	if (GameViewportClient || InGameViewportClient == nullptr)
	{
		return;
	}
	GetResourcesManager()->Reload();
	GameViewportClient =CastChecked<UStalkerGameViewportClient>(InGameViewportClient);
	GameWorld = InGameViewportClient->GetWorld();
	DelegateHandleOnViewportResized = FViewport::ViewportResizedEvent.AddUObject(this, &UStalkerEngineManager::OnViewportResized);
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
	DelegateHandleOnViewportCloseRequested = GameViewportClient->OnCloseRequested().AddUObject(this, &UStalkerEngineManager::OnViewportCloseRequested);
	Device->seqAppStart.Process(rp_AppStart);
	if (GameViewportClient->IsActive)
	{
		Device->seqAppActivate.Process(rp_AppActivate);
	}
	Device->b_is_Active = TRUE;
}

void UStalkerEngineManager::DetachViewport(class UGameViewportClient* InGameViewportClient)
{
	if (GameViewportClient&&GameViewportClient == InGameViewportClient)
	{
		Engine->Event.Defer("KERNEL:disconnect");
		Device->seqParallel.clear_not_free();
		g_Engine->OnFrame();
		Device->b_is_Active = FALSE;
		Device->seqAppEnd.Process(rp_AppEnd);
		FViewport::ViewportResizedEvent.Remove(DelegateHandleOnViewportResized);
		GameViewportClient->OnCloseRequested().Remove(DelegateHandleOnViewportCloseRequested);
		GameViewportClient = nullptr	;
		GXRaySkeletonMeshManager->Flush();
		MyXRayInput->ClearStates();
		GameWorld = nullptr;
	}
}

FName UStalkerEngineManager::GetCurrentLevel()
{
	if (MyXRayEngine)
	{
		return MyXRayEngine->CurrentLevelName;
	}
	return NAME_None;
}

void UStalkerEngineManager::Initialized()
{
	ResourcesManager = NewObject<UStalkerResourcesManager>(this);
	MyXRayInput = nullptr;
	GXRayMemory = new XRayMemory;
	GXRayDebug = new XRayDebug;
	GXRayLog = new XRayLog;
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
		CurrentGame = EStalkerGame::COP;
		FSName = TEXT("fsgame");
	}

	FSName += TEXT(".ltx");
	FSName = FPaths::Combine(FPaths::ProjectDir(), FSName);
	Core.Initialize(GXRayMemory, GXRayLog, GXRayDebug, TCHAR_TO_ANSI(*FSName), GIsEditor, GamePath);

	MyXRayEngine = new XRayEngine;
	g_Engine = MyXRayEngine;
	g_Engine->Initialize();
	GXRaySkeletonMeshManager = new XRaySkeletonMeshManager;
#if WITH_EDITOR
	DelegateHandleOnEndPlayMap=FGameDelegates::Get().GetEndPlayMapDelegate().AddUObject(this, &UStalkerEngineManager::OnEndPlayMap);
#endif
}

void UStalkerEngineManager::ReInitialized(EStalkerGame Game)
{
#if WITH_EDITOR
	if (GameWorld)
	{ 
		return;
	}
#endif
	if (GetCurrentGame() == Game)
	{
		return;
	}
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

	Core.Initialize(GXRayMemory, GXRayLog, GXRayDebug, TCHAR_TO_ANSI(*FSName), GIsEditor, GamePath);
	MyXRayEngine = new XRayEngine;
	g_Engine = MyXRayEngine;
	g_Engine->Initialize();

	ReInitializedMulticastDelegate.Broadcast();
}

void UStalkerEngineManager::Destroy()
{
#if WITH_EDITOR
	FGameDelegates::Get().GetEndPlayMapDelegate().Remove(DelegateHandleOnEndPlayMap);
#endif
	delete GXRaySkeletonMeshManager;
	GXRaySkeletonMeshManager = nullptr;
	g_Engine->Destroy();
	delete g_Engine;
	MyXRayEngine = nullptr;
	Core.Destroy();
	delete GXRayMemory;
	delete GXRayDebug;
	delete GXRayLog;
	ResourcesManager->CheckLeak();
}

void UStalkerEngineManager::SetInput(class XRayInput* InXRayInput)
{
	if (InXRayInput == nullptr) 
	{ 
		MyXRayInput = nullptr;
		return;
	}
	check(!MyXRayInput);
	MyXRayInput = InXRayInput;
}

void UStalkerEngineManager::OnViewportResized(FViewport* InViewport, uint32)

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

void UStalkerEngineManager::OnEndPlayMap()
{
#if WITH_EDITOR
	ReInitialized(GetDefault<UStalkerGameSettings>()->EditorStartupGame);
#endif

}

void UStalkerEngineManager::OnViewportCloseRequested(FViewport* InViewport)
{
	check(GameViewportClient->Viewport== InViewport);
	DetachViewport(GameViewportClient);
}
