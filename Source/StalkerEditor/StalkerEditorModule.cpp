// Copyright Epic Games, Inc. All Rights Reserved.

#include "StalkerEditorModule.h"

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
	GRayObjectLibrary = new XRayObjectLibrary;
	GRayObjectLibrary->OnCreate();
	MainMenu.Initialize();
}

void FStalkerEditorModule::ShutdownModule()
{
	MainMenu.Destroy();
	GRayObjectLibrary->OnDestroy();
	delete GRayObjectLibrary;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE(FStalkerEditorModule, StalkerEditor);