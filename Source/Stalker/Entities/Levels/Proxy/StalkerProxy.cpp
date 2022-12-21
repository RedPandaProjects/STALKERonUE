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
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

}

void AStalkerProxy::Initilize(class CObject* InXRayObject)
{
	check(XRayObject == nullptr);
	check(InXRayObject);
	XRayObject = InXRayObject;
}

// Called when the game starts or when spawned
void AStalkerProxy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStalkerProxy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!XRayObject)
	{
		return;
	}
	SetActorLocationAndRotation(FVector(StalkerMath::XRayLocationToUnreal(XRayObject->XFORM().c)), FQuat(StalkerMath::XRayQuatToUnreal(XRayObject->XFORM())));

}

AStalkerProxy* AStalkerProxy::CastToStalkerProxy()
{
	return this;
}

void AStalkerProxy::Attach(class IRenderVisual* Visual)
{
	UStalkerKinematicsComponent* StalkerKinematicsComponent  = Visual->CastToStalkerKinematicsComponent();
	check(StalkerKinematicsComponent);
	checkSlow(XRayComponents.Find(StalkerKinematicsComponent) == nullptr);
	XRayComponents.FindOrAdd(StalkerKinematicsComponent);
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, false);
	StalkerKinematicsComponent->Rename(nullptr,this);

	StalkerKinematicsComponent->AttachToComponent(GetRootComponent(), AttachmentTransformRules);
	StalkerKinematicsComponent->RegisterComponent();
}

void AStalkerProxy::Detach(class IRenderVisual* Visual)
{
	UStalkerKinematicsComponent* StalkerKinematicsComponent = Visual->CastToStalkerKinematicsComponent();
	check(StalkerKinematicsComponent);
	checkSlow(XRayComponents.Find(StalkerKinematicsComponent));
	XRayComponents.Remove(StalkerKinematicsComponent);

	FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepRelative,false);
	StalkerKinematicsComponent->DetachFromComponent(DetachmentTransformRules);
	StalkerKinematicsComponent->UnregisterComponent();
	StalkerKinematicsComponent->Rename(nullptr,GXRayEngineManager->GetResourcesManager());
}

void AStalkerProxy::DetachAll()
{
	for(USceneComponent*Component: XRayComponents)
	{
		if (Component)
		{
			FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepRelative, false);
			Component->DetachFromComponent(DetachmentTransformRules);
			Component->UnregisterComponent();
			Component->Rename(nullptr, GXRayEngineManager->GetResourcesManager());
		}
	}
	XRayComponents.Empty();
}

