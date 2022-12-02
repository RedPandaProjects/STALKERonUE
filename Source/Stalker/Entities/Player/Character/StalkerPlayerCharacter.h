#pragma once
#include "StalkerPlayerCharacter.generated.h"

UCLASS()
class AStalkerPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStalkerPlayerCharacter();


	void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XRay|Player", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
