#include "StalkerGameMode.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XR_IOConsole.h"
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
