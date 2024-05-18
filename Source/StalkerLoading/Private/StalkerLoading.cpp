#include "StalkerLoading.h"

#include "MoviePlayer.h"
#include "StalkerStartupScreen.h"

#define LOCTEXT_NAMESPACE "FStalkerLoadingModule"

void FStalkerLoadingModule::StartupModule()
{
	if (!IsRunningDedicatedServer()&&IsRunningGame())
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.WidgetLoadingScreen = SNew(SStalkerStartupScreen);
		LoadingScreen.bAllowEngineTick = true;
		LoadingScreen.bMoviesAreSkippable = true;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.MoviePaths.Add(TEXT("intro_soc_gsc"));
		LoadingScreen.MoviePaths.Add(TEXT("intro_soc_thq"));
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
		GetMoviePlayer()->PlayMovie();
	}
}

void FStalkerLoadingModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FStalkerLoadingModule, StalkerLoading)