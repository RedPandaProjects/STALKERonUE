// Fill out your copyright notice in the Description page of Project Settings.


#include "XRayGameInstance.h"
#include "../EngineManager/XRayEngineManager.h"

void UXRayGameInstance::Init()
{
	Super::Init();
}

void UXRayGameInstance::Shutdown()
{
	GXRayEngineManager->DetachViewport(GetGameViewportClient());
	Super::Shutdown();
}

void UXRayGameInstance::OnStart()
{
	Super::OnStart();
	GXRayEngineManager->AttachViewport(GetGameViewportClient());
}
