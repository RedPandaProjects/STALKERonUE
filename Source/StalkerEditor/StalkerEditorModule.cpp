// Copyright Epic Games, Inc. All Rights Reserved.

#include "StalkerEditorModule.h"
#include "StalkerEditorManager.h"
#include "UI/StalkerEditorStyle.h"
#include "UI/Commands/StalkerEditorCommands.h"
#include "UI/EdMode/AIMap/StalkerAIMapEditMode.h"
#include "UI/EdMode/WayPoints/StalkerWayObjectEditMode.h"
#include "IPlacementModeModule.h"
#include "ComponentTypeRegistry.h"
#include "Managers/SEFactory/StalkerSEFactoryManager.h"
#include "MessageLogModule.h"

#define LOCTEXT_NAMESPACE "XRayImporterModule"
DEFINE_LOG_CATEGORY(LogXRayImporter);
DEFINE_LOG_CATEGORY(LogStalkerEditor);

static void* RedImageMemoryAllocationFunction(void* pointer, size_t size)
{
	if (!size)
	{
		FMemory::Free(pointer);
		return nullptr;
	}
	if (pointer)
	{
		return 	FMemory::Realloc(pointer, size);
	}
	return FMemory::Malloc(size);
}

void FStalkerEditorModule::StartupModule()
{
	RedImageTool::MemoryAllocationFunction = &RedImageMemoryAllocationFunction;
	if(GIsEditor)
	{
		StalkerEditorCommands::Register();
		GStalkerEditorManager = NewObject< UStalkerEditorManager>();
		GStalkerEditorManager->AddToRoot();
		GStalkerEditorManager->Initialized();
		MainMenu.Initialize();
		ToolBarMenu.Initialize();
		BuildMenu.Initialize();
		PlayMenu.Initialize();
		ToolsMenu.Initialize();

		FStalkerEditorStyle::Initialize();
		FStalkerEditorStyle::ReloadTextures();
	


		FEditorModeRegistry::Get().RegisterMode<FStalkerAIMapEditMode>(
			FStalkerAIMapEditMode::EM_AIMap,FText::FromString("AIMap Editor"),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), TEXT("StalkerEditor.BuildAIMap")),
			true, 500
			);

		FEditorModeRegistry::Get().RegisterMode<FStalkerWayObjectEditMode>(
			FStalkerWayObjectEditMode::EM_WayObject, FText::FromString("Way Points Editor"),
			FSlateIcon(/*FStalkerEditorStyle::GetStyleSetName(), TEXT("StalkerEditor.BuildAIMap")*/),
			true, 500
			);
	}
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FStalkerEditorModule::OnPostEngineInit);
	FCoreDelegates::OnPreExit.AddRaw(this, &FStalkerEditorModule::OnPreExit);
}

void FStalkerEditorModule::ShutdownModule()
{
	if (GIsEditor)
	{
		if ( FModuleManager::Get().IsModuleLoaded("MessageLog") )
		{
			FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
			MessageLogModule.UnregisterLogListing("StalkerEditor");
		}
		FEditorModeRegistry::Get().UnregisterMode(FStalkerAIMapEditMode::EM_AIMap);
		FEditorModeRegistry::Get().UnregisterMode(FStalkerWayObjectEditMode::EM_WayObject);
		FStalkerEditorStyle::Shutdown();
		FCoreDelegates::OnPostEngineInit.RemoveAll(this);
		FCoreDelegates::OnPreExit.RemoveAll(this);
		PlayMenu.Destroy();
		ToolBarMenu.Destroy();
		MainMenu.Destroy();
		BuildMenu.Destroy();
		ToolsMenu.Destroy();
		StalkerEditorCommands::Unregister();
	}
}

void FStalkerEditorModule::OnPostEngineInit()
{
	GStalkerEditorManager->SEFactoryManager->Initialized();
	FComponentTypeRegistry::Get().Invalidate();
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	MessageLogModule.RegisterLogListing( "StalkerEditor",FText::FromString(TEXT("StalkerEditor Errors")));
	auto GInterchangeEnableDDSImportVar = IConsoleManager::Get().FindConsoleVariable(TEXT("Interchange.FeatureFlags.Import.DDS"));
	if (GInterchangeEnableDDSImportVar)
	{
		GInterchangeEnableDDSImportVar->Set(false);
	}
}

void FStalkerEditorModule::OnPreExit()
{
	GStalkerEditorManager->Destroy();
	GStalkerEditorManager->RemoveFromRoot();
	GStalkerEditorManager->MarkAsGarbage();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE(FStalkerEditorModule, StalkerEditor);