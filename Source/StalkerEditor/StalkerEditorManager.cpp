#include "StalkerEditorManager.h"
#include "Kernel/StalkerEngineManager.h"
#include "Managers/CFrom/StalkerEditorCForm.h"
#include "UI/StalkerEditorStyle.h"
#include "Managers/AIMap/StalkerEditorAIMap.h"
UStalkerEditorManager* GStalkerEditorManager = nullptr;

void UStalkerEditorManager::Initialized()
{
	if (GIsEditor)
	{
		GRayObjectLibrary = new XRayObjectLibrary;
		GRayObjectLibrary->OnCreate();
		if (GXRayEngineManager->GetCurrentGame() == EStalkerGame::SHOC)
		{
			SOCMaterials.Load();
		}
		GXRayEngineManager->ReInitializedMulticastDelegate.AddUObject(this, &UStalkerEditorManager::OnReInitialized);

		auto GInterchangeEnableDDSImportVar = IConsoleManager::Get().FindConsoleVariable(TEXT("Interchange.FeatureFlags.Import.DDS"));
		if (GInterchangeEnableDDSImportVar)
		{
			GInterchangeEnableDDSImportVar->Set(false);
		}

		ScanSkeletons();
		EditorCFrom = NewObject<UStalkerEditorCForm>();
		EditorCFrom->Initialize();
		EditorAIMap = NewObject<UStalkerEditorAIMap>();
		EditorAIMap->Initialize();
	
	}
}

void UStalkerEditorManager::Destroy()
{
	if (GIsEditor)
	{
		EditorAIMap->Destroy();
		EditorAIMap->MarkAsGarbage();
		EditorAIMap = nullptr;
		EditorCFrom->Destroy();
		EditorCFrom->MarkAsGarbage();
		EditorCFrom = nullptr;
		GXRayEngineManager->ReInitializedMulticastDelegate.RemoveAll(this);
		GRayObjectLibrary->OnDestroy();
		delete GRayObjectLibrary;
	}
}

FString UStalkerEditorManager::GetGamePath()
{
	switch (xrGameManager::GetGame())
	{
	case EGame::CS:
		return TEXT("/Game/CS");
	case EGame::SHOC:
		return TEXT("/Game/SHOC");
	default:
		return TEXT("/Game/COP");
	}
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

void UStalkerEditorManager::ScanSkeletons()
{
	Skeletons.Empty();
	const FString PackageName = UPackageTools::SanitizePackageName(GetGamePath());
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByPath(FName(*PackageName), AssetData, true);
	for (FAssetData& Data : AssetData)
	{
		USkeleton* Skeleton = Cast<USkeleton>(Data.GetAsset());
		if (Skeleton)
		{
			Skeletons.Add(Skeleton);
		}
	}

}

