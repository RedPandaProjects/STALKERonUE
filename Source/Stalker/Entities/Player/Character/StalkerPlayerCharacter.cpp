#include "StalkerPlayerCharacter.h"
#include "../Controller/StalkerPlayerController.h"
// Sets default values
AStalkerPlayerCharacter::AStalkerPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetEnableGravity(false);
	GetMovementComponent()->bAutoActivate = false;

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetRootComponent());
	FirstPersonCameraComponent->bUsePawnControlRotation = true; 
}

void AStalkerPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AStalkerPlayerController *PC = Cast<AStalkerPlayerController>(NewController);
	if (PC)
	{
		PC->CreateHUD();
	}
}

// Called when the game starts or when spawned
void AStalkerPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStalkerPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Fquaternion XRayQuat;
	Fmatrix View;
	View.identity();
	View.i = Device->vCameraTop;
	View.j = Device->vCameraDirection;
	View.k = Device->vCameraRight;

	XRayQuat.set(View);
	FQuat Quat(XRayQuat.x, -XRayQuat.z, -XRayQuat.y, XRayQuat.w);
	FVector Location(-Device->vCameraPosition.x * 100, Device->vCameraPosition.z * 100, Device->vCameraPosition.y * 100);
	FRotator RotationToUnreal(90,0,0);
	FRotator Rotation(Quat*FQuat(RotationToUnreal));
	CastChecked<APlayerController>(GetController())->SetControlRotation(Rotation);
	SetActorLocationAndRotation(Location,Rotation);
}

// Called to bind functionality to input
void AStalkerPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

