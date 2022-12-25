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
	FCoreDelegates::OnPreExit.AddRaw(this, &FStalkerModule::OnPreExit);

}

void FStalkerModule::ShutdownModule()
{
	FCoreDelegates::OnPreExit.RemoveAll(this);
}

void FStalkerModule::OnPreExit()
{
	GXRayEngineManager->Destroy();
	GXRayEngineManager->RemoveFromRoot();
	GXRayEngineManager->MarkAsGarbage();

}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_PRIMARY_GAME_MODULE(FStalkerModule, Stalker, "Stalker");