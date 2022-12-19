#include "StalkerEditorManager.h"
#include "Kernel/StalkerEngineManager.h"
UStalkerEditorManager* GStalkerEditorManager = nullptr;

void UStalkerEditorManager::Initialized()
{
	GRayObjectLibrary = new XRayObjectLibrary;
	GRayObjectLibrary->OnCreate();
	if (GXRayEngineManager->GetCurrentGame() == EStalkerGame::SHOC)
	{
		SOCMaterials.Load();
	}
	DelegateHandleOnReInitialized = GXRayEngineManager->ReInitializedMulticastDelegate.AddUObject(this,&UStalkerEditorManager::OnReInitialized);

	auto GInterchangeEnableDDSImportVar = IConsoleManager::Get().FindConsoleVariable(TEXT("Interchange.FeatureFlags.Import.DDS"));
	if (GInterchangeEnableDDSImportVar)
	{
		GInterchangeEnableDDSImportVar->Set(false);
	}
}

void UStalkerEditorManager::Destroy()
{
	GXRayEngineManager->ReInitializedMulticastDelegate.Remove(DelegateHandleOnReInitialized);
	GRayObjectLibrary->OnDestroy();
	delete GRayObjectLibrary;
}

void UStalkerEditorManager::OnReInitialized()
{
	GRayObjectLibrary->OnDestroy();
	GRayObjectLibrary->OnCreate();
	SOCMaterials.UnLoad();
	if (GXRayEngineManager->GetCurrentGame() == EStalkerGame::SHOC)
	{
		SOCMaterials.Load();
	}
}

