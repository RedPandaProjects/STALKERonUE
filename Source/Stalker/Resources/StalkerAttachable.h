#pragma once
#include "StalkerAttachable.generated.h"

UCLASS()
class STALKER_API UStalkerAttachable :public UObject, public XRayUnrealAttachableInterface
{
	GENERATED_BODY()

public:
			UStalkerAttachable	();
	void	AttachTo			(XRayUnrealAttachableInterface* Attach, const char* BoneName) override;
	void	Detach				() override;
	void	SetOffset			(const Fmatrix& offset) override;
	void	SetOwnerNoSee		(bool Enable) override;
	void	SetOnlyOwnerSee		(bool Enable) override;
	void	Lock				(CObject*) override;
	void	Lock				(void*) override;
	void	Unlock				(void*) override;
	void*	CastUnrealObject	(EXRayUnrealObjectType ObjectType) override;

	UPROPERTY(Transient)
	USceneComponent* OwnerComponent;
};
