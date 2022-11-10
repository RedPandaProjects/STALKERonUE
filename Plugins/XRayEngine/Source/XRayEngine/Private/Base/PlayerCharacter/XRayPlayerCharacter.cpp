// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/PlayerCharacter/XRayPlayerCharacter.h"
#include "../PlayerController/XRayPlayerController.h"

// Sets default values
AXRayPlayerCharacter::AXRayPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AXRayPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AXRayPlayerController *PC = Cast<AXRayPlayerController>(NewController);
	if (PC)
	{
		PC->CreateHUD();
	}
}

// Called when the game starts or when spawned
void AXRayPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AXRayPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AXRayPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

