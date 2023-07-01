// Copyright Epic Games, Inc. All Rights Reserved.

#include "Stalker.h"
#include "Kernel/StalkerEngineManager.h"
#include "Resources/Particle/StalkerParticleDomain.h"
#include "NiagaraTypes.h"

#define LOCTEXT_NAMESPACE "StalkerModule"
DEFINE_LOG_CATEGORY(LogStalker);

void FStalkerModule::StartupModule()
{
	GStalkerEngineManager = new FStalkerEngineManager();
	GStalkerEngineManager->Initialize();
	FCoreDelegates::OnPreExit.AddRaw(this, &FStalkerModule::OnPreExit);
	
}

void FStalkerModule::ShutdownModule()
{
	FCoreDelegates::OnPreExit.RemoveAll(this);
}

void FStalkerModule::OnPreExit()
{
	delete GStalkerEngineManager;
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_PRIMARY_GAME_MODULE(FStalkerModule, Stalker, "Stalker");