#include "StalkerProxy.h"
#include "../../Kinematics/StalkerKinematicsComponent.h"
#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"

THIRD_PARTY_INCLUDES_START
#include "XrEngine/xr_object.h"
THIRD_PARTY_INCLUDES_END
// Sets default values
AStalkerProxy::AStalkerProxy()
{
	PrimaryActorTick.bCanEverTick = true;
	XRayObject = nullptr;
}

void AStalkerProxy::Lock(class CObject* InXRayObject)
{
	check(XRayObject == nullptr);
	check(InXRayObject);
	XRayObject = InXRayObject;
#if WITH_EDITORONLY_DATA
	SetActorLabel(InXRayObject->cName().c_str());
#endif
}

void AStalkerProxy::Unlock(void* InXRayObject)
{
	check(XRayObject == InXRayObject);
	XRayObject = nullptr;
}

void AStalkerProxy::BeginPlay()
{
	Super::BeginPlay();
}

void AStalkerProxy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!XRayObject)
	{
		return;
	}
	SetActorTransform(FTransform(StalkerMath::RBMKMatrixToUnreal( XRayObject->XFORM())));
}

void AStalkerProxy::BeginDestroy()
{
	check(XRayObject == nullptr);
	Super::BeginDestroy();
}

void AStalkerProxy::Lock(void* InXRayObject)
{
	unimplemented();
}

void AStalkerProxy::AttachTo(IRBMKUnrealAttachable* AttachableInterface, const char* BoneName)
{
	USceneComponent* SceneComponent = reinterpret_cast<USceneComponent*>(AttachableInterface->CastUnrealObject(ERBMKUnrealObjectType::SceneComponent));
	check(SceneComponent);
	check(GetRootComponent());
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	check(AttachToComponent(SceneComponent,AttachmentTransformRules,BoneName));
	if (UPrimitiveComponent* PrimitiveComponen = Cast<UPrimitiveComponent>(SceneComponent))
	{
		SetOwnerNoSee(PrimitiveComponen->bOwnerNoSee);
		SetOnlyOwnerSee(PrimitiveComponen->bOnlyOwnerSee);
	}
}

void AStalkerProxy::SetAsRoot(IRBMKUnrealAttachable* AttachableInterface)
{
	TArray<USceneComponent*> InChildren;
	if (GetRootComponent())
	{
		InChildren = GetRootComponent()->GetAttachChildren();
	}
	
	check(AttachableInterface->CastUnrealObject(ERBMKUnrealObjectType::Actor)==nullptr);
	USceneComponent* SceneComponent = reinterpret_cast<USceneComponent*>(AttachableInterface->CastUnrealObject(ERBMKUnrealObjectType::SceneComponent));
	check(SceneComponent);
	if(	UStalkerKinematicsComponent* StalkerKinematicsComponent = Cast<UStalkerKinematicsComponent>(SceneComponent))
	{
		GStalkerEngineManager->GetResourcesManager()->UnregisterKinematics(StalkerKinematicsComponent);
		StalkerKinematicsComponent->Rename(nullptr,this);
		SetRootComponent(StalkerKinematicsComponent);
		StalkerKinematicsComponent->SetSimulatePhysics(false);
		StalkerKinematicsComponent->RegisterComponent();
	}
	else
	{
		SetRootComponent(SceneComponent);
	}

	for (USceneComponent*Component:InChildren)
	{
		Component->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
	}
	K2_PostSetAsRoot();
}

void AStalkerProxy::Detach()
{
	FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, false);
	DetachFromActor(DetachmentTransformRules);
}

void AStalkerProxy::SetOffset(const Fmatrix& offset)
{
	GetRootComponent()->SetRelativeTransform(FTransform(StalkerMath::RBMKMatrixToUnreal(offset)));
}

void* AStalkerProxy::CastUnrealObject(ERBMKUnrealObjectType ObjectType)
{
	switch (ObjectType)
	{
	case ERBMKUnrealObjectType::Object:
		return static_cast<UObject*>(this);
	case ERBMKUnrealObjectType::Actor:
		return static_cast<AActor*>(this);
	case ERBMKUnrealObjectType::StalkerProxy:
		return this;
	case ERBMKUnrealObjectType::SceneComponent:
		return GetRootComponent();
	default: 
		return nullptr;
	}
}

void AStalkerProxy::SetOwnerNoSee(bool Enable)
{
	if (UPrimitiveComponent* PrimitiveComponen = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		PrimitiveComponen->SetOwnerNoSee(Enable);
	}
}

void AStalkerProxy::SetOnlyOwnerSee(bool Enable)
{
	if (UPrimitiveComponent* PrimitiveComponen = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		PrimitiveComponen->SetOnlyOwnerSee(Enable);
	}
}

void AStalkerProxy::GetWorldTransform(Fmatrix& OutXForm)
{
	OutXForm = StalkerMath::UnrealMatrixToRBMK(GetRootComponent()->GetComponentToWorld().ToMatrixWithScale());
}

bool AStalkerProxy::IsAttached(IRBMKUnrealAttachable* Attach)
{
	if(USceneComponent*RootComp = reinterpret_cast<USceneComponent*>(Attach->CastUnrealObject(ERBMKUnrealObjectType::SceneComponent)))
	{
		return RootComponent->IsAttachedTo(RootComp);
	}
	return false;
}

void AStalkerProxy::SetVisibility(bool NewVisibility)
{
	SetActorHiddenInGame(!NewVisibility);
}

bool AStalkerProxy::IsStalker()
{
	if (XRayObject)
	{
		return XRayObject->IsStalker();
	}
	return false;
}
