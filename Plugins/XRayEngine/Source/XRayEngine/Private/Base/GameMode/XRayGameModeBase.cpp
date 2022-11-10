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
	FVector2D ScreenSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ScreenSize);
	if (u32(ScreenSize.X) != Device->dwWidth || u32(ScreenSize.Y) != Device->dwHeight)
	{
		Device->dwWidth = u32(ScreenSize.X);
		Device->dwHeight = u32(ScreenSize.Y);
		Device->seqResolutionChanged.Process(rp_ScreenResolutionChanged);
	}

	Device->dwTimeDelta = DeltaSeconds;
	Device->dwTimeGlobal = static_cast<u32>(GetWorld()->TimeSeconds*1000);
	Device->seqFrame.Process(rp_Frame);
	Device->dwFrame++;
}

void AXRayGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

}

void AXRayGameModeBase::StartPlay()
{
	Super::StartPlay();
	Console->Execute("main_menu on");
}

void AXRayGameModeBase::ResetLevel()
{
	Super::ResetLevel();
}

void AXRayGameModeBase::Reset()
{
	Super::Reset();
}
