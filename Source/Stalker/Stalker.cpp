// Copyright Epic Games, Inc. All Rights Reserved.

#include "Stalker.h"
#include "Kernel/StalkerEngineManager.h"

#define LOCTEXT_NAMESPACE "StalkerModule"
DEFINE_LOG_CATEGORY(LogStalker);

void FStalkerModule::StartupModule()
{
	GStalkerEngineManager = NewObject< UStalkerEngineManager>();
	GStalkerEngineManager->AddToRoot();
	GStalkerEngineManager->Initialized();
	FCoreDelegates::OnPreExit.AddRaw(this, &FStalkerModule::OnPreExit);

}

void FStalkerModule::ShutdownModule()
{
	FCoreDelegates::OnPreExit.RemoveAll(this);
}

void FStalkerModule::OnPreExit()
{
	GStalkerEngineManager->Destroy();
	GStalkerEngineManager->RemoveFromRoot();
	GStalkerEngineManager->MarkAsGarbage();

}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_PRIMARY_GAME_MODULE(FStalkerModule, Stalker, "Stalker");