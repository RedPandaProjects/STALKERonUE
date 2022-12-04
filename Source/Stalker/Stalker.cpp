// Copyright Epic Games, Inc. All Rights Reserved.

#include "Stalker.h"
#include "Kernel/StalkerEngineManager.h"

#define LOCTEXT_NAMESPACE "StalkerModule"
DEFINE_LOG_CATEGORY(LogStalker);

void FStalkerModule::StartupModule()
{
	GXRayEngineManager = NewObject< UStalkerEngineManager>();
	GXRayEngineManager->AddToRoot();
	GXRayEngineManager->Initialized();


}

void FStalkerModule::ShutdownModule()
{
	GXRayEngineManager->Destroy();
	GXRayEngineManager->RemoveFromRoot();
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_PRIMARY_GAME_MODULE(FStalkerModule, Stalker, "Stalker");