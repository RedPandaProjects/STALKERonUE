// Copyright Epic Games, Inc. All Rights Reserved.

#include "XRayImporterModule.h"

#define LOCTEXT_NAMESPACE "FXRayImporterModule"
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

void FXRayImporterModule::StartupModule()
{
	Lib.OnCreate();
	RedImageTool::MemoryAllocationFunction = &RedImageMemoryAllocationFunction;
}

void FXRayImporterModule::ShutdownModule()
{
	Lib.OnDestroy();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXRayImporterModule, XRayImporter)