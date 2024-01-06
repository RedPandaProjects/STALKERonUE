#pragma once
#include "StalkerAttachable.generated.h"

UCLASS()
class STALKER_API UStalkerAttachable :public UObject, public IRBMKUnrealAttachable
{
	GENERATED_BODY()

public:
			UStalkerAttachable	();
	void	AttachTo			(IRBMKUnrealAttachable* Attach, const char* BoneName) override;
	void	Detach				() override;
	void	SetOffset			(const Fmatrix& offset) override;
	void	SetOwnerNoSee		(bool Enable) override;
	void	SetOnlyOwnerSee		(bool Enable) override;
	void	Lock				(CObject*) override;
	void	Lock				(void*) override;
	void	Unlock				(void*) override;
	void*	CastUnrealObject	(ERBMKUnrealObjectType ObjectType) override;
	void	SetVisibility		(bool NewVisibility) override;
	
	void	GetWorldTransform	(Fmatrix& OutXForm) override;
	bool	IsAttached			(IRBMKUnrealAttachable* Attach) override;
	UPROPERTY(Transient)
	USceneComponent* OwnerComponent;
};
