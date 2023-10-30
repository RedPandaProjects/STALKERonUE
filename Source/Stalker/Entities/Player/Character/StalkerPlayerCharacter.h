#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XRayUnrealProxyInterface.h"
THIRD_PARTY_INCLUDES_END

#include "StalkerPlayerCharacter.generated.h"

UCLASS()
class AStalkerPlayerCharacter : public ACharacter, public XRayUnrealPlayerCharacterInterface
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
	void							AttachTo					(XRayUnrealAttachableInterface* Attach, const char* BoneName) override;
	void							Detach						() override;
	void							Lock						(void*) override;
	void							Lock						(class CObject*) override;
	void							Unlock						(void*) override;
	void*							CastUnrealObject			(EXRayUnrealObjectType ObjectType) override;
	void							SetAsRoot					(XRayUnrealAttachableInterface* Attachable) override;
	XRayUnrealAttachableInterface*	GetCameraComponent			() override;
	void							GetWorldTransform			(Fmatrix& OutXForm) override;
	bool							IsAttached					(XRayUnrealAttachableInterface* Attach) override;
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
};
