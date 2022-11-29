// Copyright Epic Games, Inc. All Rights Reserved.

#include "XRayImporterModule.h"

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

void FXRayImporterModule::StartupModule()
{
	RedImageTool::MemoryAllocationFunction = &RedImageMemoryAllocationFunction;
	GRayObjectLibrary = new XRayObjectLibrary;
	GRayObjectLibrary->OnCreate();
	/*for (float x = 0; x < 360.f; x += 30.f)
	{
		for (float y = 0; y < 360.f; y += 30.f)
		{
			for (float z = 0; z < 360.f; z += 45.f)
			{
				FRotator Rotation(y, z, x);
				FQuat RotationQuat(Rotation);
				FMatrix UTest = RotationQuat.ToMatrix();
				FMatrix XUTest;
				Fmatrix XTest;
				Fquaternion XQuat;
				XQuat.rotationYawPitchRoll(deg2rad(z), deg2rad(y), deg2rad(x));
				XTest.rotation(XQuat);
				for (size_t i = 0; i < 4; i++)
				{
					for (size_t j = 0; j < 4; j++)
					{
						XUTest.M[i][j] = XTest.m[j][i];
					}
				}
				UE_LOG(LogXRayImporter, Log, TEXT("Rotation [%s] Matrix[%s] XRay Matrxi[%s]"), *Rotation.ToString(), *UTest.ToString(), *XUTest.ToString());
			}
		}
	}*/
}

void FXRayImporterModule::ShutdownModule()
{
	GRayObjectLibrary->OnDestroy();
	delete GRayObjectLibrary;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXRayImporterModule, XRayImporter)