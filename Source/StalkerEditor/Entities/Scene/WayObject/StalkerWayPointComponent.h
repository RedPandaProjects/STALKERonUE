#pragma once
#include "StalkerWayPointLink.h"
#include "StalkerWayPointComponent.generated.h"


UCLASS(hideCategories = (Rendering, Physics, HLOD, Activation, Input, Actor, Cooking,Collision,Lighting,Navigation,ComponentReplication,Variable,LOD,TextureStreaming,Mobile,RayTracing,Replication))
class UStalkerWayPointComponent :public UPrimitiveComponent
{
	GENERATED_BODY()
public:
	FPrimitiveSceneProxy*		CreateSceneProxy		() override;
	FBoxSphereBounds			CalcBounds				(const FTransform& InLocalToWorld) const override;

	void						AddLink					(UStalkerWayPointComponent* Point, float Probability = 1.f);
	void						AddDoubleLink			(UStalkerWayPointComponent* Point);
	bool						CheckLink				(UStalkerWayPointComponent* Point, float *Probability = nullptr);
	void						RemoveLink				(UStalkerWayPointComponent* Point);
	void						InvertLink				(UStalkerWayPointComponent* Point);
	void						Convert1Link			(UStalkerWayPointComponent* Point);
	void						Convert2Link			(UStalkerWayPointComponent* Point);
	void						RemoveSelectLink		();
	FBox						GetPointBox				();
	void						PreEditChange			(FProperty* PropertyThatWillChange) override;
	void						PostEditChangeProperty	(FPropertyChangedEvent& PropertyChangedEvent) override;
	bool						Modify					(bool bAlwaysMarkDirty = true) override;

	UPROPERTY(EditAnywhere, Category = "WayPoint")
	FString						PointName;

	UPROPERTY(EditAnywhere, Category = "WayPoint",Meta=(Bitmask))
	int32						Flags;

	UPROPERTY(EditInstanceOnly, EditFixedSize, NoClear, Category = "WayPoint")
	TArray<FStalkerWayPointLink>Links;

	UPROPERTY(Transient)
	bool						bIsSelected = false;
	UPROPERTY()
	bool						bIsMain = false;
	UPROPERTY(Transient)
	int32						Index =-1;
private:
	FString						TempPointName;
};