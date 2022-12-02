// Copyright Epic Games, Inc. All Rights Reserved.

#include "Stalker.h"
#define LOCTEXT_NAMESPACE "StalkerModule"
DEFINE_LOG_CATEGORY(LogStalker);

void FXRayEngineModule::StartupModule()
{
	GXRayEngineManager = NewObject< UStalkerEngineManager>();
	GXRayEngineManager->AddToRoot();
	GXRayEngineManager->Initialized();


}

void FXRayEngineModule::ShutdownModule()
{
	GXRayEngineManager->Destroy();
	GXRayEngineManager->RemoveFromRoot();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXRayEngineModule, XRayEngine)