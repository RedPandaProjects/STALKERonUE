// Copyright Epic Games, Inc. All Rights Reserved.

#include "XRayEngineModule.h"
#include "Core/XRayMemory.h"
#include "Core/XRayLog.h"
#include "Core/XRayDebug.h"
#define LOCTEXT_NAMESPACE "FXRayEngineModule"

DEFINE_LOG_CATEGORY(LogXRayEngine);
ENGINE_API int EngineLaunch(EGamePath Game);
/*PrimeCalculateAsyncTask is the name of our task
FNonAbandonableTask is the name of the class I've located from the source code of the engine*/
class PrimeCalculationAsyncTask : public FNonAbandonableTask
{
public:
	/*Default constructor*/
	PrimeCalculationAsyncTask()
	{
	}

	/*This function is needed from the API of the engine.
	My guess is that it provides necessary information
	about the thread that we occupy and the progress of our task*/
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(PrimeCalculationAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}
	/*This function is executed when we tell our task to execute*/
	void DoWork()
	{
		EngineLaunch(EGamePath::NONE);
		GLog->Log("--------------------------------------------------------------------");
		GLog->Log("End of prime numbers calculation on background thread");
		GLog->Log("--------------------------------------------------------------------");
	}
};

void FXRayEngineModule::StartupModule()
{
	GXRayMemory = new XRayMemory;
	GXRayDebug = new XRayDebug;
	GXRayLog = new XRayLog;
	FString FSName = FPaths::ProjectDir();
	if (GIsEditor)
	{
		FSName = FPaths::Combine(FSName, TEXT("fs.ltx"));
	}
	else
	{
		FSName = FPaths::Combine(FSName, TEXT("fsgame.ltx"));
	}
	Core.Initialize(GXRayMemory, GXRayLog, GXRayDebug, TCHAR_TO_ANSI(*FSName), GIsEditor, EGamePath::COP_1602);
	//(new FAutoDeleteAsyncTask<PrimeCalculationAsyncTask>())->StartBackgroundTask();
}

void FXRayEngineModule::ShutdownModule()
{
	Core.Destroy();
	delete GXRayMemory;
	delete GXRayDebug;
	delete GXRayLog;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXRayEngineModule, XRayEngine)