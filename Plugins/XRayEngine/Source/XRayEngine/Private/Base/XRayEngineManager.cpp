// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/XRayEngineManager.h"
#include "Resources/XRayResourcesManager.h"
#include "Core/XRayMemory.h"
#include "Core/XRayLog.h"
#include "Core/XRayDebug.h"
#include "Core/XRayEngine.h"
#include "Resources/XRayResourcesManager.h"
#include "Render/Resources/SkeletonMesh/XRaySkeletonMeshManager.h"
#include "Base/XRayGameViewportClient.h"
#include "Core/XRayInput.h"
THIRD_PARTY_INCLUDES_START
#include "../Source/XrEngine/XrDeviceInterface.h"
THIRD_PARTY_INCLUDES_END

UXRayEngineManager* GXRayEngineManager = nullptr;

void UXRayEngineManager::AttachViewport(class UGameViewportClient* InGameViewportClient)
{
	if (GameViewportClient || InGameViewportClient == nullptr)
	{
		return;
	}
	GetResourcesManager()->Reload();
	GameViewportClient =CastChecked<UXRayGameViewportClient>(InGameViewportClient);
	GameWorld = InGameViewportClient->GetWorld();
	DelegateHandleOnViewportResized = FViewport::ViewportResizedEvent.AddUObject(this, &UXRayEngineManager::OnViewportResized);
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
	DelegateHandleOnViewportCloseRequested = GameViewportClient->OnCloseRequested().AddUObject(this, &UXRayEngineManager::OnViewportCloseRequested);
	Device->seqAppStart.Process(rp_AppStart);
	if (GameViewportClient->IsActive)
	{
		Device->seqAppActivate.Process(rp_AppActivate);
	}
	Device->b_is_Active = TRUE;
}

void UXRayEngineManager::DetachViewport(class UGameViewportClient* InGameViewportClient)
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

void UXRayEngineManager::Initialized()
{
	ResourcesManager = NewObject<UXRayResourcesManager>(this);
	GXRayInput = nullptr;
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

void UXRayEngineManager::Destroy()
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

void UXRayEngineManager::SetInput(class XRayInput* InXRayInput)
{
	if (InXRayInput == nullptr) 
	{ 
		MyXRayInput = nullptr;
		return;
	}
	check(!MyXRayInput);
	MyXRayInput = InXRayInput;
}

void UXRayEngineManager::OnViewportResized(FViewport* InViewport, uint32)

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

void UXRayEngineManager::OnViewportCloseRequested(FViewport* InViewport)
{
	check(GameViewportClient->Viewport== InViewport);
	DetachViewport(GameViewportClient);
}
