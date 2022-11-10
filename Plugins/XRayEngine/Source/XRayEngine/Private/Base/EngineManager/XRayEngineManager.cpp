// Fill out your copyright notice in the Description page of Project Settings.


#include "XRayEngineManager.h"
#include "../ResourcesManager/XRayResourcesManager.h"
#include "Core/XRayMemory.h"
#include "Core/XRayLog.h"
#include "Core/XRayDebug.h"
#include "Core/XRayEngine.h"
#include "Base/ResourcesManager/XRayResourcesManager.h"
UXRayEngineManager* GXRayEngineManager = nullptr;

void UXRayEngineManager::AttachViewport(class UGameViewportClient* InGameViewportClient)
{
	if (GameViewportClient || InGameViewportClient == nullptr)
	{
		return;
	}
	GameViewportClient = InGameViewportClient;
	DelegateHandleOnViewportResized = FViewport::ViewportResizedEvent.AddUObject(this, &UXRayEngineManager::OnViewportResized);
}

void UXRayEngineManager::DetachViewport(class UGameViewportClient* InGameViewportClient)
{
	if (GameViewportClient == InGameViewportClient)
	{
		FViewport::ViewportResizedEvent.Remove(DelegateHandleOnViewportResized);
		GameViewportClient = nullptr;
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


	//FViewport::ViewportResizedEvent.AddUObject(this, &UGameEngine::OnViewportResized);
}

void UXRayEngineManager::Destroy()
{
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
		UE_DEBUG_BREAK();
	}
}
