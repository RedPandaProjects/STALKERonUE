#include "StalkerPlayerCharacter.h"
#include "../Controller/StalkerPlayerController.h"
#include "../../Kinematics/StalkerKinematicsComponent.h"
#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerAttachable.h"
#include "Resources/StalkerResourcesManager.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/xr_object.h"
THIRD_PARTY_INCLUDES_END
AStalkerPlayerCharacter::AStalkerPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
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
	FirstPersonCameraAttachable = NewObject<UStalkerAttachable>(this);
	FirstPersonCameraAttachable->OwnerComponent = FirstPersonCameraComponent;
	
}

// Called every frame
void AStalkerPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!XRayObject)
	{
		return;
	}
	SetActorTransform(FTransform(StalkerMath::RBMKMatrixToUnreal(XRayObject->XFORM())));

	Fmatrix InView = Device->mView;
	InView.c.set(Device->vCameraPosition);
	FTransform View = FTransform(StalkerMath::RBMKMatrixToUnreal(InView));

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

void AStalkerPlayerCharacter::SetOffset(const Fmatrix& offset)
{
}

void AStalkerPlayerCharacter::SetOwnerNoSee(bool Enable)
{
}

void AStalkerPlayerCharacter::SetOnlyOwnerSee(bool Enable)
{
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

void* AStalkerPlayerCharacter::CastUnrealObject(EXRayUnrealObjectType ObjectType)
{
	switch (ObjectType)
	{
	case EXRayUnrealObjectType::Object:
		return static_cast<UObject*>(this);
	case EXRayUnrealObjectType::Actor:
		return static_cast<AActor*>(this);
	case EXRayUnrealObjectType::StalkerPlayerCharacter:
		return static_cast<AStalkerPlayerCharacter*>(this);
	case EXRayUnrealObjectType::SceneComponent:
		{
			const TArray<TObjectPtr<USceneComponent>>& Components = GetMesh()->GetAttachChildren();
			if(ensure(Components.Num() == 1))
			{
				return static_cast<USceneComponent*>(Components[0]);
			}
			return static_cast<USceneComponent*>(GetRootComponent());
		}
	default: 
		return nullptr;
	}
}

void AStalkerPlayerCharacter::SetAsRoot(XRayUnrealAttachableInterface* AttachableInterface)
{
	check(AttachableInterface->CastUnrealObject(EXRayUnrealObjectType::Actor)==nullptr);
	USceneComponent* SceneComponent = reinterpret_cast<USceneComponent*>(AttachableInterface->CastUnrealObject(EXRayUnrealObjectType::SceneComponent));
	check(SceneComponent);
	if(	UStalkerKinematicsComponent* StalkerKinematicsComponent = Cast<UStalkerKinematicsComponent>(SceneComponent))
	{
		GStalkerEngineManager->GetResourcesManager()->UnregisterKinematics(StalkerKinematicsComponent);
		StalkerKinematicsComponent->Rename(nullptr,this);
		FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
		StalkerKinematicsComponent->AttachToComponent(GetMesh(),AttachmentTransformRules);
		StalkerKinematicsComponent->SetSimulatePhysics(false);
		StalkerKinematicsComponent->RegisterComponent();
	}
	else
	{
		FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
		AttachToComponent(GetMesh(),AttachmentTransformRules);
	}
	AddInstanceComponent(SceneComponent);
}


void AStalkerPlayerCharacter::Unlock(void*InXRayObject)
{
	check(XRayObject == InXRayObject);
	XRayObject = nullptr;
}

XRayUnrealAttachableInterface* AStalkerPlayerCharacter::GetCameraComponent()
{
	return FirstPersonCameraAttachable;
}

bool AStalkerPlayerCharacter::IsAttached(XRayUnrealAttachableInterface* Attach)
{
	if(USceneComponent*RootComp = reinterpret_cast<USceneComponent*>(Attach->CastUnrealObject(EXRayUnrealObjectType::SceneComponent)))
	{
		return RootComponent->IsAttachedTo(RootComp);
	}
	return false;
}

void AStalkerPlayerCharacter::SetVisibility(bool NewVisibility)
{
	SetActorHiddenInGame(!NewVisibility);
}

void AStalkerPlayerCharacter::GetWorldTransform(Fmatrix& OutXForm)
{
	OutXForm = StalkerMath::UnrealMatrixToRBMK(GetRootComponent()->GetComponentToWorld().ToMatrixWithScale());
}

void AStalkerPlayerCharacter::AttachTo(XRayUnrealAttachableInterface* Attach, const char* BoneName)
{
	unimplemented();
}

void AStalkerPlayerCharacter::Detach()
{
	unimplemented();
}

void AStalkerPlayerCharacter::Lock(void*)
{
	unimplemented();
}

