#include "StalkerEditorManager.h"
#include "Kernel/StalkerEngineManager.h"
#include "Managers/CFrom/StalkerEditorCForm.h"
#include "UI/StalkerEditorStyle.h"
#include "Managers/AIMap/StalkerEditorAIMap.h"
#include "Entities/EditorRender/StalkerEditorRenderProxy.h"
#include "PlacementMode/Public/IPlacementModeModule.h"
#include "Entities/Scene/SpawnObject/StalkerSpawnObject.h"
#include "Entities/Scene/SpawnObject/StalkerSpawnObjectFactory.h"
#include "Managers/SEFactory/StalkerSEFactoryManager.h"
///////////////////////////////////////////////////////////////////////////////////////////
#include "Entities/Scene/SpawnObject/Properties/StalkerSpawnPropertiesTypes.h"
#include "Entities/Scene/SpawnObject/Properties/StalkerSpawnPropertiesTypeCustomization.h"
///////////////////////////////////////////////////////////////////////////////////////////
UStalkerEditorManager* GStalkerEditorManager = nullptr;
#define LOCTEXT_NAMESPACE "StalkerEditor"
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
		GXRayEngineManager->PostReInitializedMulticastDelegate.AddUObject(this, &UStalkerEditorManager::OnReInitialized);

		auto GInterchangeEnableDDSImportVar = IConsoleManager::Get().FindConsoleVariable(TEXT("Interchange.FeatureFlags.Import.DDS"));
		if (GInterchangeEnableDDSImportVar)
		{
			GInterchangeEnableDDSImportVar->Set(false);
		}

		ScanSkeletons();
		EditorCFrom = NewObject<UStalkerEditorCForm>(this);
		EditorCFrom->Initialize();
		EditorAIMap = NewObject<UStalkerEditorAIMap>(this);
		EditorAIMap->Initialize();
		SEFactoryManager = NewObject<UStalkerSEFactoryManager>(this);

		FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UStalkerEditorManager::OnPostWorldInitialization);

		{
			FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_Storys::StaticStruct()->GetFName(),	FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_StorysCustomization::MakeInstance));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_SpawnStorys::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_SpawnStorysCustomization::MakeInstance));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_Location1::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_LocationCustomization::MakeInstance, 0));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_Location2::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_LocationCustomization::MakeInstance, 1));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_Location3::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_LocationCustomization::MakeInstance, 2));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_Location4::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_LocationCustomization::MakeInstance, 3));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_LightAnim::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_LightAnimCustomization::MakeInstance));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_CharacterProfile::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_CharacterProfileCustomization::MakeInstance));
			PropertyModule.RegisterCustomPropertyTypeLayout(FStalkerSpawnData_SmartCoverDescription::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStalkerSpawnData_SmartCoverDescriptionCustomization::MakeInstance));

		}
	}
}

void UStalkerEditorManager::Destroy()
{
	if (GIsEditor)
	{
		if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
		{
			FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_Storys::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_SpawnStorys::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_Location1::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_Location2::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_Location3::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_Location4::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_LightAnim::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_CharacterProfile::StaticStruct()->GetFName());
			PropertyModule.UnregisterCustomPropertyTypeLayout(FStalkerSpawnData_SmartCoverDescription::StaticStruct()->GetFName());
		}
		FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
		SEFactoryManager->UnLoad();
		SEFactoryManager->Destroy();
		SEFactoryManager->MarkAsGarbage();
		SEFactoryManager = nullptr;
		EditorAIMap->Destroy();
		EditorAIMap->MarkAsGarbage();
		EditorAIMap = nullptr;
		EditorCFrom->Destroy();
		EditorCFrom->MarkAsGarbage();
		EditorCFrom = nullptr;
		GXRayEngineManager->PostReInitializedMulticastDelegate.RemoveAll(this);
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

void UStalkerEditorManager::OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues)
{
	for (TActorIterator<AStalkerEditorRenderProxy> AactorItr(World); AactorItr; ++AactorItr)
	{
		if (AactorItr)
		{
			return;
		}
	}
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.ObjectFlags = RF_Transient;
	World->SpawnActor< AStalkerEditorRenderProxy>(ActorSpawnParameters);
}




#undef LOCTEXT_NAMESPACE