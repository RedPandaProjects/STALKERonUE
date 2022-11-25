// Fill out your copyright notice in the Description page of Project Settings.


#include "XRayEngineManager.h"
#include "../ResourcesManager/XRayResourcesManager.h"
#include "Core/XRayMemory.h"
#include "Core/XRayLog.h"
#include "Core/XRayDebug.h"
#include "Core/XRayEngine.h"
#include "Base/ResourcesManager/XRayResourcesManager.h"
#include "Render/Resources/SkeletonMesh/XRaySkeletonMeshManager.h"
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
	GameViewportClient = InGameViewportClient;
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
	DelegateHandleOnTick = GameViewportClient->OnTick().AddUObject(this, &UXRayEngineManager::OnTick);

	Device->seqAppStart.Process(rp_AppStart);
	Device->b_is_Active = TRUE;
}

void UXRayEngineManager::DetachViewport(class UGameViewportClient* InGameViewportClient)
{
	if (GameViewportClient&&GameViewportClient == InGameViewportClient)
	{
		Engine->Event.Defer("KERNEL:disconnect");
		g_Engine->OnFrame();
		Device->b_is_Active = FALSE;
		Device->seqAppEnd.Process(rp_AppEnd);
		FViewport::ViewportResizedEvent.Remove(DelegateHandleOnViewportResized);
		GameViewportClient->OnTick().Remove(DelegateHandleOnTick);
		GameViewportClient->OnCloseRequested().Remove(DelegateHandleOnViewportCloseRequested);
		GameViewportClient = nullptr	;
		GameWorld = nullptr;
	}
}

void UXRayEngineManager::Initialized()
{
	ResourcesManager = NewObject<UXRayResourcesManager>(this);
	
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
	g_Engine->Destroy();
	delete g_Engine;
	Core.Destroy();
	delete GXRayMemory;
	delete GXRayDebug;
	delete GXRayLog;
	ResourcesManager->CheckLeak();
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

void UXRayEngineManager::OnTick(float Delta)
{
	if (g_loading_events->size())
	{
		if (g_loading_events->front()())
			g_loading_events->pop_front();
		return;
	}
	else
	{
		Device->fTimeDelta = Delta;
		Device->fTimeGlobal = GameWorld->TimeSeconds;
		Device->dwTimeDelta = static_cast<u32>(Delta * 1000);
		Device->dwTimeGlobal = static_cast<u32>(GameWorld->TimeSeconds * 1000);
		Device->dwTimeContinual = static_cast<u32>(GameWorld->UnpausedTimeSeconds * 1000);
		g_Engine->OnFrame();
		Device->dwFrame++;
		GXRaySkeletonMeshManager->Flush();
	}

}
