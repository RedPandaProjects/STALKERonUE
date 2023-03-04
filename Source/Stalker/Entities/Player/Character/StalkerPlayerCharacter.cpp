#include "StalkerPlayerCharacter.h"
#include "../Controller/StalkerPlayerController.h"
#include "../../Kinematics/StalkerKinematics.h"
#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/xr_object.h"
THIRD_PARTY_INCLUDES_END
AStalkerPlayerCharacter::AStalkerPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetEnableGravity(false);
	GetMovementComponent()->bAutoActivate = false;

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetRootComponent());
	FirstPersonCameraComponent->bUsePawnControlRotation = true; 
	XRayObject = nullptr;
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
	if (!XRayObject)
	{
		return;
	}
	SetActorLocationAndRotation(FVector(StalkerMath::XRayLocationToUnreal(XRayObject->XFORM().c)), FQuat(StalkerMath::XRayQuatToUnreal(XRayObject->XFORM())));

	Fquaternion XRayQuat;
	Fmatrix View;
	View.identity();
	View.i = Device->vCameraTop;
	View.j = Device->vCameraDirection;
	View.k = Device->vCameraRight;

	XRayQuat.set(View);
	FQuat Quat(XRayQuat.x, -XRayQuat.z, -XRayQuat.y, XRayQuat.w);
	FVector Location(-Device->vCameraPosition.x * 100, Device->vCameraPosition.z * 100, Device->vCameraPosition.y * 100);
	FirstPersonCameraComponent->SetWorldLocationAndRotation(Location,Quat);
	FRotator RotationToUnreal(90,0,0);
	FRotator Rotation(Quat*FQuat(RotationToUnreal));
	CastChecked<APlayerController>(GetController())->SetControlRotation(Rotation);
}

void AStalkerPlayerCharacter::BeginDestroy()
{
	check(XRayObject == nullptr);
	Super::BeginDestroy();
}

// Called to bind functionality to input
void AStalkerPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AStalkerPlayerCharacter::AttachAsRoot(class IRenderVisual* Visual)
{
	UStalkerKinematicsComponent* StalkerKinematicsComponent  = Visual->CastToStalkerKinematicsComponent();
	check(StalkerKinematicsComponent);

	GXRayEngineManager->GetResourcesManager()->UnregisterKinematics(StalkerKinematicsComponent);
	StalkerKinematicsComponent->Rename(nullptr,this);
	
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	StalkerKinematicsComponent->AttachToComponent(GetRootComponent(),AttachmentTransformRules);
	StalkerKinematicsComponent->SetSimulatePhysics(false);
	StalkerKinematicsComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StalkerKinematicsComponent->RegisterComponent();
	StalkerRootComponent = StalkerKinematicsComponent;
	AddInstanceComponent(StalkerKinematicsComponent);
}

void AStalkerPlayerCharacter::Detach(class IRenderVisual* Visual)
{
	UStalkerKinematicsComponent* StalkerKinematicsComponent = Visual->CastToStalkerKinematicsComponent();
	check(StalkerKinematicsComponent);
	if (StalkerKinematicsComponent == StalkerRootComponent)
	{
		StalkerRootComponent = nullptr;
	}
	FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepRelative, false);
	RemoveInstanceComponent(StalkerKinematicsComponent);
	StalkerKinematicsComponent->DetachFromComponent(DetachmentTransformRules);
	StalkerKinematicsComponent->UnregisterComponent();
	GXRayEngineManager->GetResourcesManager()->RegisterKinematics(StalkerKinematicsComponent);
}

void AStalkerPlayerCharacter::Attach(class IRenderVisual* Visual, const char* BoneName)
{
	UStalkerKinematicsComponent* StalkerKinematicsComponent  = Visual->CastToStalkerKinematicsComponent();
	check(StalkerKinematicsComponent);

	GXRayEngineManager->GetResourcesManager()->UnregisterKinematics(StalkerKinematicsComponent);
	StalkerKinematicsComponent->Rename(nullptr,this);

	
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	StalkerKinematicsComponent->AttachToComponent(StalkerRootComponent?StalkerRootComponent:GetRootComponent(),AttachmentTransformRules,BoneName);
	StalkerKinematicsComponent->SetSimulatePhysics(false);
	StalkerKinematicsComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StalkerKinematicsComponent->RegisterComponent();
	AddInstanceComponent(StalkerKinematicsComponent);
}

class AStalkerProxy* AStalkerPlayerCharacter::CastToStalkerProxy()
{
	return nullptr;
}

class AStalkerPlayerCharacter* AStalkerPlayerCharacter::CastToStalkerPlayerCharacter()
{
	return this;
}

void AStalkerPlayerCharacter::Lock(class CObject* InXRayObject)
{
	check(XRayObject == nullptr);
	check(InXRayObject);
	XRayObject = InXRayObject;
#if WITH_EDITORONLY_DATA
	SetActorLabel(InXRayObject->cName().c_str());
#endif
}

void AStalkerPlayerCharacter::Unlock(class CObject*InXRayObject)
{
	check(XRayObject == InXRayObject);
	XRayObject = nullptr;
}

