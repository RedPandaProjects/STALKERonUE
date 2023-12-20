#include "StalkerPlayerCharacter.h"
#include "../Controller/StalkerPlayerController.h"
#include "../../Kinematics/StalkerKinematicsComponent.h"
#include "Kernel/StalkerEngineManager.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"
#include "Kernel/XRay/Render/Interface/XRayRenderTarget.h"
#include "Kismet/KismetMaterialLibrary.h"
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
	NoiseTime = -1;
	NoiseShift.X = 0;
	NoiseShift.Y = 0;
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
	PostProcessMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GWorld,GetDefault<UStalkerGameSettings>()->PostProcessMaterial.LoadSynchronous(),NAME_None,EMIDCreationFlags::Transient);

	FWeightedBlendable& WeightedBlendable = FirstPersonCameraComponent->PostProcessSettings.WeightedBlendables.Array. AddDefaulted_GetRef();
	WeightedBlendable.Object = PostProcessMaterial;
	WeightedBlendable.Weight = 1;	NoiseTime = -1;
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
	const FRotator RotationToUnreal(0,90,0);
	const FRotator Rotation(FQuat(View.GetRotation())*FQuat(RotationToUnreal));
	CastChecked<APlayerController>(GetController())->SetControlRotation(Rotation);

	static FName NAME_Brightness = "Brightness";
	PostProcessMaterial->SetVectorParameterValue(NAME_Brightness,GRenderTarget.AddColor);

	static FName NAME_Gray = "Gray";
	FLinearColor GrayColor = GRenderTarget.GrayColor;
	GrayColor.A = FMath::Clamp(1 - GRenderTarget.Gray,0,1);
	PostProcessMaterial->SetVectorParameterValue(NAME_Gray,GrayColor);

	FLinearColor BaseColor = GRenderTarget.BaseColor;
	BaseColor.A = FMath::Clamp(1 - GRenderTarget.Noise,0,1);
	PostProcessMaterial->SetVectorParameterValue(NAME_Color,BaseColor);

	const FVector2f MagicTextureSize = {128,128};
	auto CalculateUVNoiseLambda = [this,DeltaTime,MagicTextureSize]()
	{
		const FVector2f VirtualTextureSize = MagicTextureSize*GRenderTarget.NoiseScale+EPS_S;
		NoiseTime -= DeltaTime;
		if(NoiseTime<0)
		{
			int32 Width = FMath::CeilToInt(VirtualTextureSize.X);
			Width = FMath::RandHelper(Width?Width:1);
			int32 Height = FMath::CeilToInt(VirtualTextureSize.Y);
			Height = FMath::RandHelper(Height?Height:1);
			if(FMath::IsNearlyZero(GRenderTarget.NoiseFPS))
			{
				NoiseTime = 1;
			}
			else
			{
				while (NoiseTime<0)		
				{
					NoiseTime += 1.f/GRenderTarget.NoiseFPS;
				}
			}
			NoiseShift.X = static_cast<float>(Width);
			NoiseShift.Y = static_cast<float>(Height);
		}
		const FVector2f ScreenSize = {static_cast<float>(GRenderTarget.get_width()),static_cast<float>(GRenderTarget.get_height())};
		const FVector2f StartUV = (NoiseShift+0.5f)/VirtualTextureSize;
		const FVector2f EndUV = StartUV + (VirtualTextureSize/ScreenSize) + 1.f;
		return FVector4f{StartUV.X,StartUV.Y,EndUV.X,EndUV.Y};
	};
	
	static FName NAME_NoiseUV = "NoiseUV";
	PostProcessMaterial->SetVectorParameterValue(NAME_NoiseUV,CalculateUVNoiseLambda());

	auto CalculateUVDualityLambda = [this](FVector4f&OutUV1,FVector4f&OutUV2)
	{
		const FVector2f ScreenSize = {static_cast<float>(GRenderTarget.get_width()),static_cast<float>(GRenderTarget.get_height())};
		const FVector2f Shift = (FVector2f(0.5f) / ScreenSize)*GRenderTarget.Blur;
		const FVector2f Blur0 = (FVector2f(0.5f) / ScreenSize) + Shift;
		const FVector2f Blur1 = (FVector2f(0.5f) + ScreenSize)/ScreenSize + Shift;
		const FVector2f ShiftDuality = GRenderTarget.Duality*0.5f;
		OutUV1.X = Blur0.X;
		OutUV1.Y = Blur0.Y;
		OutUV1.Z = Blur1.X - ShiftDuality.X;
		OutUV1.W = Blur1.Y - ShiftDuality.Y;
		
		OutUV2.X = Blur0.X + ShiftDuality.X;
		OutUV2.Y = Blur0.Y + ShiftDuality.Y;
		OutUV2.Z = Blur1.X;
		OutUV2.W = Blur1.Y;
	};
	FVector4f ScreenUV1,ScreenUV2;
	CalculateUVDualityLambda(ScreenUV1,ScreenUV2);
	static FName NAME_SceenUV1 = "SceenUV1";
	PostProcessMaterial->SetVectorParameterValue(NAME_SceenUV1,ScreenUV1);
	static FName NAME_SceenUV2 = "SceenUV2";
	PostProcessMaterial->SetVectorParameterValue(NAME_SceenUV2,ScreenUV2);
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

