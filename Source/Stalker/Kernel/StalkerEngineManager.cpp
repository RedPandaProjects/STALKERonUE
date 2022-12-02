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

UStalkerEngineManager* GXRayEngineManager = nullptr;

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

void UStalkerEngineManager::Initialized()
{
	ResourcesManager = NewObject<UStalkerResourcesManager>(this);
	MyXRayInput = nullptr;
	GXRayMemory = new XRayMemory;
	GXRayDebug = new XRayDebug;
	GXRayLog = new XRayLog;
	FString FSName = FPaths::ProjectDir();
	if (GIsEditor)
	{
		FSName = FPaths::Combine(FSName, TEXT("fs.ltx"));
	} 
	else
	{
		FSName = FPaths::Combine(FSName, TEXT("fsgame.ltx"));
	}
	Core.Initialize(GXRayMemory, GXRayLog, GXRayDebug, TCHAR_TO_ANSI(*FSName), GIsEditor, EGamePath::COP_1602);

	g_Engine = new XRayEngine;
	g_Engine->Initialize();
	GXRaySkeletonMeshManager = new XRaySkeletonMeshManager;
	
	//FViewport::ViewportResizedEvent.AddUObject(this, &UGameEngine::OnViewportResized);
}

void UStalkerEngineManager::Destroy()
{
	delete GXRaySkeletonMeshManager;
	GXRaySkeletonMeshManager = nullptr;
	g_Engine->Destroy();
	delete g_Engine;
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

void UStalkerEngineManager::OnViewportCloseRequested(FViewport* InViewport)
{
	check(GameViewportClient->Viewport== InViewport);
	DetachViewport(GameViewportClient);
}
