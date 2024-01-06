#pragma once

#include "StalkerPlayerCharacter.generated.h"

UCLASS()
class AStalkerPlayerCharacter : public ACharacter, public IRBMKUnrealPlayerCharacter
{
	GENERATED_BODY()

public:
									AStalkerPlayerCharacter		();
	void							PossessedBy					(AController* NewController) override;
	virtual void					Tick						(float DeltaTime) override;
	void							BeginDestroy				() override;
	virtual void					SetupPlayerInputComponent	(class UInputComponent* PlayerInputComponent) override;

	
	void							SetOffset					(const Fmatrix& offset) override;
	void							SetOwnerNoSee				(bool Enable) override;
	void							SetOnlyOwnerSee				(bool Enable) override;
	void							AttachTo					(IRBMKUnrealAttachable* Attach, const char* BoneName) override;
	void							Detach						() override;
	void							Lock						(void*) override;
	void							Lock						(class CObject*) override;
	void							Unlock						(void*) override;
	void*							CastUnrealObject			(ERBMKUnrealObjectType ObjectType) override;
	void							SetAsRoot					(IRBMKUnrealAttachable* Attachable) override;
	IRBMKUnrealAttachable*			GetCameraComponent			() override;
	void							GetWorldTransform			(Fmatrix& OutXForm) override;
	bool							IsAttached					(IRBMKUnrealAttachable* Attach) override;
	void							SetVisibility				(bool NewVisibility) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stalker|Player", meta = (AllowPrivateAccess = "true"))
	UCameraComponent*			FirstPersonCameraComponent;
	UPROPERTY(Transient)
	class UStalkerAttachable*	FirstPersonCameraAttachable;

protected:
	virtual void				BeginPlay					() override;

public:

private:
	class CObject*				XRayObject;

	UPROPERTY()
	USceneComponent			*	StalkerRootComponent;

	UPROPERTY(Transient,VisibleAnywhere)
	UMaterialInstanceDynamic*	PostProcessMaterial;

	UPROPERTY(Transient)
	float NoiseTime;

	UPROPERTY(Transient)
	FVector2f	NoiseShift;
};
