#include "StalkerAttachable.h"

UStalkerAttachable::UStalkerAttachable()
{
}

void UStalkerAttachable::AttachTo(IRBMKUnrealAttachable* AttachableInterface, const char* BoneName)
{
	USceneComponent* SceneComponent = reinterpret_cast<USceneComponent*>(AttachableInterface->CastUnrealObject(ERBMKUnrealObjectType::SceneComponent));
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
	FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, false);
	OwnerComponent->DetachFromComponent(DetachmentTransformRules);
}

void UStalkerAttachable::SetOffset(const Fmatrix& offset)
{
	OwnerComponent->SetRelativeTransform(FTransform(StalkerMath::RBMKMatrixToUnreal(offset)));
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

void* UStalkerAttachable::CastUnrealObject(ERBMKUnrealObjectType ObjectType)
{
	switch (ObjectType)
	{
	case ERBMKUnrealObjectType::Object:
		return static_cast<UObject*>(OwnerComponent);
	case ERBMKUnrealObjectType::SceneComponent:
		return OwnerComponent;
	default: 
		return nullptr;
	}
}

void UStalkerAttachable::SetVisibility(bool NewVisibility)
{
	OwnerComponent->SetVisibility(NewVisibility);
}

void UStalkerAttachable::GetWorldTransform(Fmatrix& OutXForm)
{
	OutXForm = StalkerMath::UnrealMatrixToRBMK(OwnerComponent->GetComponentToWorld().ToMatrixWithScale());
}

bool UStalkerAttachable::IsAttached(IRBMKUnrealAttachable* Attach)
{
	if(USceneComponent*RootComp = reinterpret_cast<USceneComponent*>(Attach->CastUnrealObject(ERBMKUnrealObjectType::SceneComponent)))
	{
		return OwnerComponent->IsAttachedTo(RootComp);
	}
	return false;
}

