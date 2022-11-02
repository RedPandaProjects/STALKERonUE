// Copyright Epic Games, Inc. All Rights Reserved.

#include "XRayEngine.h"
#include "XrCore/stdafx.h"
#include "Async/AsyncWork.h"
#define LOCTEXT_NAMESPACE "FXRayEngineModule"

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
		Core._initialize("xray", NULL, TRUE, "E:\\GameDev\\Perforce\\Stalker\\fsgame.ltx", false, EGamePath::COP_1602);
		EngineLaunch(EGamePath::NONE);
		GLog->Log("--------------------------------------------------------------------");
		GLog->Log("End of prime numbers calculation on background thread");
		GLog->Log("--------------------------------------------------------------------");
	}
};

void FXRayEngineModule::StartupModule()
{

	(new FAutoDeleteAsyncTask<PrimeCalculationAsyncTask>())->StartBackgroundTask();
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FXRayEngineModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXRayEngineModule, XRayEngine)