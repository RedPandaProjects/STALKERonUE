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
	void						PossessedBy					(AController* NewController) override;
	virtual void				Tick						(float DeltaTime) override;
	void						BeginDestroy				() override;
	virtual void				SetupPlayerInputComponent	(class UInputComponent* PlayerInputComponent) override;
	void						AttachAsRoot				(class IRenderVisual* Visual) override;
	void						Attach						(class IRenderVisual* Visual, const char* BoneName) override;
	void						AttachToCamera				(class IRenderVisual* Visual) override;
	class AStalkerProxy*		CastToStalkerProxy			() override;
	AStalkerPlayerCharacter*	CastToStalkerPlayerCharacter() override;
	void						Lock						(class CObject*) override;
	void						Unlock						(class CObject*) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stalker|Player", meta = (AllowPrivateAccess = "true"))
	UCameraComponent*			FirstPersonCameraComponent;


protected:
	virtual void				BeginPlay					() override;
private:
	class CObject*				XRayObject;
	UPROPERTY()
	USceneComponent			*	StalkerRootComponent;
};
