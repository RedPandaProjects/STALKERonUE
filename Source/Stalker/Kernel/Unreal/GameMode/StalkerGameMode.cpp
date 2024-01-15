#include "StalkerGameMode.h"

#include "Entities/Levels/Environment/StalkerEnvironment.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XR_IOConsole.h"
#include "XrEngine/IGame_Level.h"
THIRD_PARTY_INCLUDES_END

#include "Entities/Player/Character/StalkerPlayerCharacter.h"
#include "Entities/Player/Controller/StalkerPlayerController.h"
#include "Entities/Debug/StalkerDebugRender.h"
#include "../WorldSettings/StalkerWorldSettings.h"

AStalkerGameMode::AStalkerGameMode(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	DefaultPawnClass = AStalkerPlayerCharacter::StaticClass();
	PlayerControllerClass = AStalkerPlayerController::StaticClass();
	StalkerEnvironmentClass = AStalkerEnvironment::StaticClass();
}

void AStalkerGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AStalkerGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	{	
		UWorld* World = GetWorld();

		AStalkerWorldSettings* WorldSettings = Cast<AStalkerWorldSettings>( World->GetWorldSettings());
		if (WorldSettings)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Instigator = GetInstigator();
			SpawnInfo.ObjectFlags |= RF_Transient;
			WorldSettings->DebugRender = World->SpawnActor<AStalkerDebugRender>(AStalkerDebugRender::StaticClass(), SpawnInfo);

			if(UClass* InStalkerEnvironmentClass = StalkerEnvironmentClass.LoadSynchronous())
			{
				WorldSettings->Environment = World->SpawnActor<AStalkerEnvironment>(InStalkerEnvironmentClass, SpawnInfo);
			}
			else
			{
				WorldSettings->Environment = World->SpawnActor<AStalkerEnvironment>(SpawnInfo);
			}
		}
	}
}

void AStalkerGameMode::StartPlay()
{
	Super::StartPlay();
}

void AStalkerGameMode::ResetLevel()
{
	Super::ResetLevel();
}

void AStalkerGameMode::Reset()
{
	Super::Reset();
}

bool AStalkerGameMode::ReadyToStartMatch_Implementation()
{
	if (!Super::ReadyToStartMatch_Implementation())
	{
		return false;
	}
	if (AStalkerWorldSettings* WorldSettings = Cast<AStalkerWorldSettings>(GetWorld()->GetWorldSettings()))
	{
		if (WorldSettings->NotForXRay)
		{
			return Device->b_is_Active;
		}
		if (!g_pGameLevel)
		{
			return false;
		}
		if (!g_pGameLevel->GetActor())
		{
			return false;
		}
		if (!g_loading_events->size())
		{
			return true;
		}
		return false;
	}
	return true;
}