// Copyright Epic Games, Inc. All Rights Reserved.

#include "StalkerEditorModule.h"
#include "StalkerEditorManager.h"
#include "UI/StalkerEditorStyle.h"
#include "UI/Commands/StalkerEditorCommands.h"

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

		FCoreDelegates::OnPreExit.AddRaw(this, &FStalkerEditorModule::OnPreExit);
	}
}

void FStalkerEditorModule::ShutdownModule()
{
	if (GIsEditor)
	{
		ToolBarMenu.Destroy();
		MainMenu.Destroy();
		BuildMenu.Destroy();
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