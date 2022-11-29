// Copyright Epic Games, Inc. All Rights Reserved.

#include "XRayEngineModule.h"
#include "Base/XRayEngineManager.h"
#define LOCTEXT_NAMESPACE "FXRayEngineModule"
DEFINE_LOG_CATEGORY(LogXRayEngine);

void FXRayEngineModule::StartupModule()
{
	GXRayEngineManager = NewObject< UXRayEngineManager>();
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