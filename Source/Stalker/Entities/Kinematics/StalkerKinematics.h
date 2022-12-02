#pragma once
#include "StalkerKinematics.generated.h"

UCLASS()
class AStalkerKinematics : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStalkerKinematics();
	void SetKinematics(class XRayKinematics* Kinematics);
	// Called every frame
	void Tick(float DeltaTime) override;
	void PostInitProperties() override;
protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;
	UPROPERTY(EditAnywhere)
	USceneComponent* SceneComponent;
private:
	class XRayKinematics* Kinematics;

};
