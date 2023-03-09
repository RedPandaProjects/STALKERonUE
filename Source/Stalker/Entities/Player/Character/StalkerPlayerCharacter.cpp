#include "StalkerPlayerCharacter.h"
#include "../Controller/StalkerPlayerController.h"
#include "../../Kinematics/StalkerKinematicsComponent.h"
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
	SetActorTransform(FTransform(StalkerMath::XRayMatrixToUnreal(XRayObject->XFORM())));

	Fmatrix InView = Device->mView;
	InView.c.set(Device->vCameraPosition);
	FTransform View = FTransform(StalkerMath::XRayMatrixToUnreal(InView));

	View.SetRotation(View.GetRotation().Inverse());
	FirstPersonCameraComponent->SetWorldTransform(View);
	FirstPersonCameraComponent->FieldOfView = Device->fFOV;
	FRotator RotationToUnreal(0,90,0);
	FRotator Rotation(FQuat(View.GetRotation())*FQuat(RotationToUnreal));
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

	GStalkerEngineManager->GetResourcesManager()->UnregisterKinematics(StalkerKinematicsComponent);
	StalkerKinematicsComponent->Rename(nullptr,this);
	
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	StalkerKinematicsComponent->AttachToComponent(GetRootComponent(),AttachmentTransformRules);
	StalkerKinematicsComponent->SetSimulatePhysics(false);
	StalkerKinematicsComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StalkerKinematicsComponent->RegisterComponent();
	StalkerRootComponent = StalkerKinematicsComponent;
	AddInstanceComponent(StalkerKinematicsComponent);
}


void AStalkerPlayerCharacter::Attach(class IRenderVisual* Visual, const char* BoneName)
{
	UStalkerKinematicsComponent* StalkerKinematicsComponent  = Visual->CastToStalkerKinematicsComponent();
	check(StalkerKinematicsComponent);

	GStalkerEngineManager->GetResourcesManager()->UnregisterKinematics(StalkerKinematicsComponent);
	StalkerKinematicsComponent->Rename(nullptr,this);

	
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	StalkerKinematicsComponent->AttachToComponent(StalkerRootComponent?StalkerRootComponent:GetRootComponent(),AttachmentTransformRules,BoneName);
	StalkerKinematicsComponent->SetSimulatePhysics(false);
	StalkerKinematicsComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StalkerKinematicsComponent->RegisterComponent();
	if (UPrimitiveComponent* PrimitiveComponen = Cast<UPrimitiveComponent>(StalkerRootComponent))
	{
		StalkerKinematicsComponent->SetOwnerNoSee(PrimitiveComponen->bOwnerNoSee);
		StalkerKinematicsComponent->SetOnlyOwnerSee(PrimitiveComponen->bOnlyOwnerSee);
	}

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

void AStalkerPlayerCharacter::AttachToCamera(class IRenderVisual* Visual)
{
	UStalkerKinematicsComponent* StalkerKinematicsComponent  = Visual->CastToStalkerKinematicsComponent();
	check(StalkerKinematicsComponent);

	GStalkerEngineManager->GetResourcesManager()->UnregisterKinematics(StalkerKinematicsComponent);
	StalkerKinematicsComponent->Rename(nullptr,this);
	
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	StalkerKinematicsComponent->AttachToComponent(FirstPersonCameraComponent,AttachmentTransformRules);
	StalkerKinematicsComponent->SetSimulatePhysics(false);
	StalkerKinematicsComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StalkerKinematicsComponent->RegisterComponent();
	AddInstanceComponent(StalkerKinematicsComponent);
}

