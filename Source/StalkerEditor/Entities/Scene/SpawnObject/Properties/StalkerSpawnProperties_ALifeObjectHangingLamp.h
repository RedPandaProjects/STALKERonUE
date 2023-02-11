#pragma once
#include "StalkerSpawnProperties_ALifeObject.h"
#include "StalkerSpawnProperties_ALifeObjectHangingLamp.generated.h"

UCLASS()
class  UStalkerSpawnProperties_ALifeObjectHangingLamp : public UStalkerSpawnProperties_ALifeObject
{
	GENERATED_BODY()
public:
	void				SetEntity(ISE_Abstract* InEntity) override;
	void				FillProperties() override;
	void				PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "Light")
	FColor				Color;

	UPROPERTY(EditAnywhere, Category = "Light")
	float				Intensity;

	UPROPERTY(EditAnywhere, Category = "Light")
	float				Range;

	UPROPERTY(EditAnywhere, Category = "Light")
	TObjectPtr<class UMaterialInterface> LightFunctionMaterial;

	UPROPERTY(EditAnywhere, Category = "Light")
	FString				AttachBone;

	UPROPERTY(EditAnywhere, Category = "Light")
	FStalkerSpawnData_LightAnim				ColorAnimator;

	UPROPERTY(EditAnywhere, Category = "Light")
	float				SpotConeAngle;

	UPROPERTY(EditAnywhere, Category = "Light")
	TArray<FString>		FixedBones;

	UPROPERTY(EditAnywhere, Category = "Light")
	bool				Physic;

	UPROPERTY(EditAnywhere, Category = "Light")
	bool				CastShadow;

	UPROPERTY(EditAnywhere, Category = "Light")
	bool				IsSpot;

	UPROPERTY(EditAnywhere, Category = "Light")
	float				Health;
protected:
	ISE_ALifeObjectHangingLamp* ALifeObjectHangingLamp;
};