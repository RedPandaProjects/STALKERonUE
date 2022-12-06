#pragma once
#include "StalkerKinematicsLegacy.generated.h"

UCLASS()
class AStalkerKinematicsLegacy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStalkerKinematicsLegacy();
	void SetKinematics(class XRayKinematicsLegacy* Kinematics);
	// Called every frame
	void Tick(float DeltaTime) override;
	void PostInitProperties() override;
protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;
	UPROPERTY(EditAnywhere)
	USceneComponent* SceneComponent;
private:
	class XRayKinematicsLegacy* Kinematics;

};
