// Copyright Epic Games, Inc. All Rights Reserved.

#include "StalkerEditorModule.h"
#include "StalkerEditorManager.h"
#include "UI/StalkerEditorStyle.h"
#include "UI/Commands/StalkerEditorCommands.h"
#include "UI/EdMode/AIMap/StalkerAIMapEditMode.h"
#include "UI/EdMode/WayPoints/StalkerWayObjectEditMode.h"

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

		FStalkerEditorStyle::Initialize();
		FStalkerEditorStyle::ReloadTextures();
		MainMenu.Initialize();
		ToolBarMenu.Initialize();
		BuildMenu.Initialize();


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
	FCoreDelegates::OnPreExit.AddRaw(this, &FStalkerEditorModule::OnPreExit);
}

void FStalkerEditorModule::ShutdownModule()
{
	if (GIsEditor)
	{
		ToolBarMenu.Destroy();
		MainMenu.Destroy();
		BuildMenu.Destroy();
		FEditorModeRegistry::Get().UnregisterMode(FStalkerAIMapEditMode::EM_AIMap);
		FEditorModeRegistry::Get().UnregisterMode(FStalkerWayObjectEditMode::EM_WayObject);
		FStalkerEditorStyle::Shutdown();
		FCoreDelegates::OnPreExit.RemoveAll(this);
		StalkerEditorCommands::Unregister();
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