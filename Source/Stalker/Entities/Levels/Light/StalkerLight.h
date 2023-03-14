// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StalkerLight.generated.h"

UCLASS()
class STALKER_API AStalkerLight : public AActor,public IRender_Light
{
	GENERATED_BODY()
	
public:	
	AStalkerLight();
	void Tick(float DeltaTime) override;
	void Lock();
	void Unlock();
	void BeginDestroy() override;

	void set_type(LT type) override;
	void set_active(bool) override;
	bool get_active() override;
	void set_shadow(bool) override;
	void set_position(const Fvector& P) override;
	void set_rotation(const Fvector& D, const Fvector& R) override;
	void set_cone(float angle) override;
	void set_range(float R) override;
	void set_intensity(float InIntensity);
	void set_material(LPCSTR name) override;
	void set_color(const Fcolor& C) override;
	void set_color(float r, float g, float b) override;
	void set_hud_mode(bool b) override;
	bool get_hud_mode() override;
	void set_volumetric_intensity(float Inintensity) override;

protected:
	void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere)
	class UPointLightComponent*LightPoint;
	UPROPERTY(VisibleAnywhere)
	class USpotLightComponent* SpotPoint;
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* SceneComponent;
	bool					   IsLocked = false;
	LT						   LightType;
	FVector					   StartPosition;
};
