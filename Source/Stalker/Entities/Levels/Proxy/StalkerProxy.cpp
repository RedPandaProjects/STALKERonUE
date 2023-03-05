#include "StalkerProxy.h"
#include "../../Kinematics/StalkerKinematics.h"
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

void AStalkerProxy::Unlock(class CObject* InXRayObject)
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
	SetActorLocationAndRotation(FVector(StalkerMath::XRayLocationToUnreal(XRayObject->XFORM().c)), FQuat(StalkerMath::XRayQuatToUnreal(XRayObject->XFORM())));

}

class AStalkerPlayerCharacter* AStalkerProxy::CastToStalkerPlayerCharacter()
{
	return nullptr;
}

void AStalkerProxy::Attach(class IRenderVisual* Visual, const char* BoneName)
{	
	UStalkerKinematicsComponent* StalkerKinematicsComponent  = Visual->CastToStalkerKinematicsComponent();
	check(StalkerKinematicsComponent);
	check(GetRootComponent());

	GXRayEngineManager->GetResourcesManager()->UnregisterKinematics(StalkerKinematicsComponent);
	StalkerKinematicsComponent->Rename(nullptr,this);
	//Set
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	StalkerKinematicsComponent->AttachToComponent(GetRootComponent(),AttachmentTransformRules,BoneName);
	StalkerKinematicsComponent->SetSimulatePhysics(false);
	StalkerKinematicsComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StalkerKinematicsComponent->RegisterComponent();
	
	if (UPrimitiveComponent* PrimitiveComponen = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		StalkerKinematicsComponent->SetOwnerNoSee(PrimitiveComponen->bOwnerNoSee);
		StalkerKinematicsComponent->SetOnlyOwnerSee(PrimitiveComponen->bOnlyOwnerSee);
	}

	AddInstanceComponent(StalkerKinematicsComponent);
}

void AStalkerProxy::BeginDestroy()
{
	check(XRayObject == nullptr);
	Super::BeginDestroy();
}

AStalkerProxy* AStalkerProxy::CastToStalkerProxy()
{
	return this;
}

void AStalkerProxy::AttachAsRoot(class IRenderVisual* Visual)
{
	UStalkerKinematicsComponent* StalkerKinematicsComponent  = Visual->CastToStalkerKinematicsComponent();
	check(StalkerKinematicsComponent);
	GXRayEngineManager->GetResourcesManager()->UnregisterKinematics(StalkerKinematicsComponent);
	StalkerKinematicsComponent->Rename(nullptr,this);
	SetRootComponent(StalkerKinematicsComponent);
	StalkerKinematicsComponent->SetSimulatePhysics(false);
	StalkerKinematicsComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StalkerKinematicsComponent->RegisterComponent();
}
