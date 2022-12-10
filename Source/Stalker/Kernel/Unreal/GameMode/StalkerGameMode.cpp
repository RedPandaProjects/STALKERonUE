#include "StalkerGameMode.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XR_IOConsole.h"
THIRD_PARTY_INCLUDES_END

#include "Entities/Player/Character/StalkerPlayerCharacter.h"
#include "Entities/Player/Controller/StalkerPlayerController.h"

AStalkerGameMode::AStalkerGameMode(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
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
