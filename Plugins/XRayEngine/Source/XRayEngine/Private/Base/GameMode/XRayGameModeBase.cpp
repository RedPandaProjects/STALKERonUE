// Fill out your copyright notice in the Description page of Project Settings.


#include "XRayGameModeBase.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XR_IOConsole.h"
THIRD_PARTY_INCLUDES_END

#include "../PlayerCharacter/XRayPlayerCharacter.h"
#include "../PlayerController/XRayPlayerController.h"
AXRayGameModeBase::AXRayGameModeBase(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	DefaultPawnClass = AXRayPlayerCharacter::StaticClass();
	PlayerControllerClass = AXRayPlayerController::StaticClass();
}

void AXRayGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AXRayGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

}

void AXRayGameModeBase::StartPlay()
{
	Super::StartPlay();
	Engine->Event.Defer("KERNEL:start", u64(xr_strdup("editor/single/alife/new")), u64(xr_strdup("localhost")));
}

void AXRayGameModeBase::ResetLevel()
{
	Super::ResetLevel();
}

void AXRayGameModeBase::Reset()
{
	Super::Reset();
}
