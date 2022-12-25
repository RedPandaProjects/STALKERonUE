// Copyright Epic Games, Inc. All Rights Reserved.

#include "StalkerEditorModule.h"
#include "StalkerEditorManager.h"

#define LOCTEXT_NAMESPACE "XRayImporterModule"
DEFINE_LOG_CATEGORY(LogXRayImporter);

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
	GStalkerEditorManager = NewObject< UStalkerEditorManager>();
	GStalkerEditorManager->AddToRoot();
	GStalkerEditorManager->Initialized();
	MainMenu.Initialize();
	FCoreDelegates::OnPreExit.AddRaw(this, &FStalkerEditorModule::OnPreExit);
}

void FStalkerEditorModule::ShutdownModule()
{
	MainMenu.Destroy();
	FCoreDelegates::OnPreExit.RemoveAll(this);
}

void FStalkerEditorModule::OnPreExit()
{

	GStalkerEditorManager->Destroy();
	GStalkerEditorManager->RemoveFromRoot();
	GStalkerEditorManager->MarkAsGarbage();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE(FStalkerEditorModule, StalkerEditor);