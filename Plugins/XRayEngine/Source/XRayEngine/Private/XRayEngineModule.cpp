// Copyright Epic Games, Inc. All Rights Reserved.

#include "XRayEngineModule.h"
#include "Core/XRayMemory.h"
#include "Core/XRayLog.h"
#include "Core/XRayDebug.h"
#include "Core/XRayEngine.h"
#include "Base/ResourcesManager/XRayResourcesManager.h"
#define LOCTEXT_NAMESPACE "FXRayEngineModule"
DEFINE_LOG_CATEGORY(LogXRayEngine);

void FXRayEngineModule::StartupModule()
{
	GXRayResourcesManager = NewObject< UXRayResourcesManager>();
	GXRayResourcesManager->AddToRoot();

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

}

void FXRayEngineModule::ShutdownModule()
{
	g_Engine->Destroy();
	delete g_Engine;
	Core.Destroy();
	delete GXRayMemory;
	delete GXRayDebug;
	delete GXRayLog;

	GXRayResourcesManager->RemoveFromRoot();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXRayEngineModule, XRayEngine)