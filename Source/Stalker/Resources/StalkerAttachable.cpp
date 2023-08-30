#include "StalkerAttachable.h"

UStalkerAttachable::UStalkerAttachable()
{
}

void UStalkerAttachable::AttachTo(XRayUnrealAttachableInterface* AttachableInterface, const char* BoneName)
{
	USceneComponent* SceneComponent = reinterpret_cast<USceneComponent*>(AttachableInterface->CastUnrealObject(EXRayUnrealObjectType::SceneComponent));
	check(SceneComponent);
	check(OwnerComponent);
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	check(OwnerComponent->AttachToComponent(SceneComponent,AttachmentTransformRules,BoneName));
	if (UPrimitiveComponent* PrimitiveComponen = Cast<UPrimitiveComponent>(SceneComponent))
	{
		SetOwnerNoSee(PrimitiveComponen->bOwnerNoSee);
		SetOnlyOwnerSee(PrimitiveComponen->bOnlyOwnerSee);
	}
}

void UStalkerAttachable::Detach()
{
	FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepRelative, false);
	OwnerComponent->DetachFromComponent(DetachmentTransformRules);
}

void UStalkerAttachable::SetOffset(const Fmatrix& offset)
{
	OwnerComponent->SetRelativeTransform(FTransform(StalkerMath::XRayMatrixToUnreal(offset)));
}

void UStalkerAttachable::SetOwnerNoSee(bool Enable)
{
	if (UPrimitiveComponent* PrimitiveComponen = Cast<UPrimitiveComponent>(OwnerComponent))
	{
		PrimitiveComponen->SetOwnerNoSee(Enable);
	}
}

void UStalkerAttachable::SetOnlyOwnerSee(bool Enable)
{
	if (UPrimitiveComponent* PrimitiveComponen = Cast<UPrimitiveComponent>(OwnerComponent))
	{
		PrimitiveComponen->SetOnlyOwnerSee(Enable);
	}
}

void UStalkerAttachable::Lock(CObject*)
{
	unimplemented();
}

void UStalkerAttachable::Lock(void*)
{
	unimplemented();
}

void UStalkerAttachable::Unlock(void*)
{
	unimplemented();
}

void* UStalkerAttachable::CastUnrealObject(EXRayUnrealObjectType ObjectType)
{
	switch (ObjectType)
	{
	case EXRayUnrealObjectType::Object:
		return static_cast<UObject*>(OwnerComponent);
	case EXRayUnrealObjectType::SceneComponent:
		return OwnerComponent;
	default: 
		return nullptr;
	}
}
