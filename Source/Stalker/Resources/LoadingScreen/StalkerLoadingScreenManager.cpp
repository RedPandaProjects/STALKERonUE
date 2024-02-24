#include "StalkerLoadingScreenManager.h"

#include <MoviePlayer.h>

#include "Kernel/StalkerEngineManager.h"
#include "Kernel/Unreal/UI/LoadingScreen/StalkerBaseLoadingScreen.h"
#include "Kernel/Unreal/UI/LoadingScreen/SOC/StalkerSOCLoadingIntroScreen.h"
#include "Kernel/Unreal/UI/LoadingScreen/SOC/StalkerSOCLoadingScreen.h"
#include "Windows/WindowsPlatformInput.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStalkerLoadingScreenManager, Log, All);
DEFINE_LOG_CATEGORY(LogStalkerLoadingScreenManager);

FStalkerLoadingInputProcessor::FStalkerLoadingInputProcessor(class FStalkerLoadingScreenManager*InOwner):Owner(InOwner)
{
}

FStalkerLoadingInputProcessor::~FStalkerLoadingInputProcessor()
{
}

bool FStalkerLoadingInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	Owner->UpdateData.Enqueue({EStalkerLoadingUpdateDataType::PressAnyKey});
	return false;
}

bool FStalkerLoadingInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp,const FPointerEvent& MouseEvent)
{
	Owner->UpdateData.Enqueue({EStalkerLoadingUpdateDataType::PressAnyKey});
	return false;
}

void FStalkerLoadingInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
}

FStalkerLoadingScreenManager::FStalkerLoadingScreenManager():BackgroundImage(nullptr),MapImage(nullptr),MainFont(nullptr)
{

}

FStalkerLoadingScreenManager::~FStalkerLoadingScreenManager()
{
}

void FStalkerLoadingScreenManager::Initialize()
{
	BackgroundImage = LoadObject<UTexture2D>(nullptr,	*(GStalkerEngineManager->GetGamePath()/TEXT("Textures/ui/ui_load.ui_load")));
	ensure(BackgroundImage);
	MainFont = LoadObject<UFont>(nullptr,TEXT("/Game/Base/Fonts/Graffiti.Graffiti"));
	ensure(MainFont);
	LoadingInputProcessor = MakeShared<FStalkerLoadingInputProcessor>(this);
}

void FStalkerLoadingScreenManager::AddReferencedObjects(FReferenceCollector& Collector)
{
	if(BackgroundImage)
	{
		Collector.AddReferencedObject(BackgroundImage);
	}
	if(MapImage)
	{
		Collector.AddReferencedObject(MapImage);
	}
	if(MainFont)
	{
		Collector.AddReferencedObject(MainFont);
	}
}

FString FStalkerLoadingScreenManager::GetReferencerName() const
{
	return TEXT("FStalkerLoadingScreenManager");
}


void FStalkerLoadingScreenManager::SetTitle(const char* InTitle)
{
	FString Title;
	{
		const int sizeNeeded = ::MultiByteToWideChar(CP_ACP, 0, InTitle, FCStringAnsi::Strlen(InTitle), NULL, 0);
		wchar_t* strTo = new wchar_t[static_cast<size_t>(sizeNeeded+1)];
		::MultiByteToWideChar(CP_ACP, 0, InTitle, FCStringAnsi::Strlen(InTitle), strTo, sizeNeeded);
		strTo[sizeNeeded] = 0;
		Title = strTo;
		delete strTo;
	}
	UE_LOG(LogStalkerLoadingScreenManager,Log,TEXT("[%d]%s"),StatusIndex,*Title);
	UpdateData.Enqueue({EStalkerLoadingUpdateDataType::NextStatus,Title,nullptr,StatusIndex});
	StatusIndex++;
}

void FStalkerLoadingScreenManager::Play(bool IsNewGame,int32 MaxStatus)
{
	if (!IsRunningDedicatedServer())
	{
		switch (GStalkerEngineManager->GetCurrentGame())
		{
		case EStalkerGame::SHOC:
			{
				StatusIndex = 0;
				UpdateData.Empty();
				;
				if(IsNewGame)
				{
					FLoadingScreenAttributes LoadingScreen;
					LoadingScreen.WidgetLoadingScreen = SNew(SStalkerSOCLoadingIntroScreen);
					LoadingScreen.bAllowEngineTick = true;
					LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
					LoadingScreen.bMoviesAreSkippable = false;
					LoadingScreen.bWaitForManualStop = true;
					LoadingScreen.MoviePaths.Add(TEXT("intro_soc_stalker"));
					FSlateApplication::Get().RegisterInputPreProcessor(LoadingInputProcessor);
					GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
					GetMoviePlayer()->PlayMovie();
				}
				else
				{
					FLoadingScreenAttributes LoadingScreen;
					LoadingScreen.WidgetLoadingScreen = SNew(SStalkerSOCLoadingScreen,MaxStatus);
					LoadingScreen.bAllowEngineTick = true;
					LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
					LoadingScreen.bWaitForManualStop = true;
					GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
					GetMoviePlayer()->PlayMovie();
				}
				
			}
			break;
		default:
			PlayDefault();
			break;
		}
		
		
	}
}

void FStalkerLoadingScreenManager::PlayDefault()
{
	if (!IsRunningDedicatedServer())
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.WidgetLoadingScreen = SNew(SStalkerBaseLoadingScreen);
		LoadingScreen.bAllowEngineTick = true;
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
		GetMoviePlayer()->PlayMovie();
	}
}

void FStalkerLoadingScreenManager::SetMapImage(FString Name)
{
	MapImage = LoadObject<UTexture2D>(nullptr,	*(GStalkerEngineManager->GetGamePath()/FString::Printf(TEXT("Textures/intro/intro_%s.intro_%s"),*Name,*Name)));
	if(!MapImage)
	{
		MapImage = LoadObject<UTexture2D>(nullptr,	*(GStalkerEngineManager->GetGamePath()/TEXT("Textures/intro/intro_no_start_picture.intro_no_start_picture")));
		if(!ensure(MapImage))
		{
			return;
		}
	}
	UpdateData.Enqueue({EStalkerLoadingUpdateDataType::SetMapImage,TEXT(""),MapImage});
}

void FStalkerLoadingScreenManager::Wait()
{
	if (!IsRunningDedicatedServer())
	{
		UpdateData.Enqueue({EStalkerLoadingUpdateDataType::EndLoad});
		GetMoviePlayer()->WaitForMovieToFinish();
		if(FSlateApplication::Get().FindInputPreProcessor(LoadingInputProcessor)!=INDEX_NONE)
		{
			FSlateApplication::Get().UnregisterInputPreProcessor(LoadingInputProcessor);
		}
		{
			FLoadingScreenAttributes LoadingScreen;
			LoadingScreen.WidgetLoadingScreen = SNew(SStalkerBaseLoadingScreen);
			LoadingScreen.bAllowEngineTick = false;
			GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
		}
	}
}
