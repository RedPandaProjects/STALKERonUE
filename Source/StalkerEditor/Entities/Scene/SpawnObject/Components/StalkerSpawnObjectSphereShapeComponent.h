#pragma once
#include "StalkerSpawnObjectSphereShapeComponent.generated.h"


UCLASS(hideCategories = (Rendering, Physics, HLOD, Activation, Input, Actor, Cooking, Collision, Lighting, Navigation, ComponentReplication, Variable, LOD, TextureStreaming, Mobile, RayTracing, Replication), ClassGroup = (Stalker), meta = (DisplayName = "Stalker Sphere Shape", BlueprintSpawnableComponent))
class UStalkerSpawnObjectSphereShapeComponent :public UPrimitiveComponent
{
	GENERATED_BODY()
public:
	FPrimitiveSceneProxy*		CreateSceneProxy	() override;
	FBoxSphereBounds			CalcBounds			(const FTransform& InLocalToWorld) const override;
	void						UpdateColor			();
	void						OnComponentCreated	() override;
	bool						Modify				(bool bAlwaysMarkDirty = true) override;
	
	UPROPERTY()
	FColor						ShapeColor			;

	UPROPERTY(EditAnywhere,Category="Shape")
	float						SphereRadius = 100;



};