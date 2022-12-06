// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Levels/Light/StalkerLight.h"

// Sets default values
AStalkerLight::AStalkerLight()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);

	LightPoint = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point"));
	LightPoint->SetupAttachment(GetRootComponent());
	LightPoint->bAutoActivate = false;

	SpotPoint = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spot"));
	SpotPoint->SetupAttachment(GetRootComponent());
	SpotPoint->bAutoActivate = false;
	LightType = IRender_Light::DIRECT;
	SpotPoint->SetRelativeRotation(FRotator(0,90,0));
}

// Called when the game starts or when spawned
void AStalkerLight::BeginPlay()
{
	Super::BeginPlay();
	SetActorLocation(StartPosition);
	
}

void AStalkerLight::set_type(LT type)
{
	set_active(false);
	LightType = type;
}

void AStalkerLight::set_active(bool InActive)
{
	switch (LightType)
	{
	case IRender_Light::POINT:
		LightPoint->SetActive(InActive);
		break;
	case IRender_Light::SPOT:
		SpotPoint->SetActive(InActive);
		break;
	}
}

bool AStalkerLight::get_active()
{
	switch (LightType)
	{
	case IRender_Light::POINT:
		return LightPoint->IsActive();
		break;
	case IRender_Light::SPOT:
		return SpotPoint->IsActive();
		break;
	}
	return false;
}

void AStalkerLight::set_shadow(bool InShadow)
{
	switch (LightType)
	{
	case IRender_Light::POINT:
		LightPoint->SetCastShadows(InShadow);
		break;
	case IRender_Light::SPOT:
		SpotPoint->SetCastShadows(InShadow);
		break;
	}
}

void AStalkerLight::set_volumetric(bool)
{
}

void AStalkerLight::set_volumetric_quality(float)
{
}

void AStalkerLight::set_volumetric_intensity(float)
{
}

void AStalkerLight::set_volumetric_distance(float)
{
}

void AStalkerLight::set_indirect(bool)
{
}

void AStalkerLight::set_position(const Fvector& P)
{
	StartPosition = FVector(StalkerMath::XRayLocationToUnreal(P))+FVector(0,0,-10);
	SetActorLocation(StartPosition);
}

void AStalkerLight::set_rotation(const Fvector& D, const Fvector& R)
{
	Fvector					L_dir, L_up, L_right;

	// dir
	L_dir.set(D);
	float l_dir_m = L_dir.magnitude();
	if (_valid(l_dir_m) && l_dir_m > EPS_S)	L_dir.div(l_dir_m);
	else									L_dir.set(0, 0, 1);

	// R&N
	if (R.square_magnitude() > EPS) {
		// use specified 'up' and 'right', just enshure ortho-normalization
		L_right.set(R);				L_right.normalize();
		L_up.crossproduct(L_dir, L_right);		L_up.normalize();
		L_right.crossproduct(L_up, L_dir);			L_right.normalize();
	}
	else {
		// auto find 'up' and 'right' vectors
		L_up.set(0, 1, 0);				if (_abs(L_up.dotproduct(L_dir)) > .99f)	L_up.set(0, 0, 1);
		L_right.crossproduct(L_up, L_dir);			L_right.normalize();
		L_up.crossproduct(L_dir, L_right);		L_up.normalize();
	}

	// matrix
	Fmatrix					mR;
	mR.i = L_right;	mR._14 = 0;
	mR.j = L_up;		mR._24 = 0;
	mR.k = L_dir;	mR._34 = 0;
	mR.c.set(0,0,0);	mR._44 = 1;

	FQuat4f Quat = StalkerMath::XRayQuatToUnreal(mR);
	SetActorRotation(FQuat(Quat));
}

void AStalkerLight::set_cone(float angle)
{
	SpotPoint->SetInnerConeAngle(angle);
	SpotPoint->SetOuterConeAngle(angle);
}

void AStalkerLight::set_range(float R)
{
	if (R < 0.01f)
	{
		set_active(false);
	}
	switch (LightType)
	{
	case IRender_Light::POINT:
		LightPoint->SetSourceRadius(R * 100);
		break;
	case IRender_Light::SPOT:
		SpotPoint->SetSourceRadius(R * 100);
		break;
	}
}

void AStalkerLight::set_virtual_size(float R)
{
}

void AStalkerLight::set_texture(LPCSTR name)
{
}

void AStalkerLight::set_color(const Fcolor& C)
{
	switch (LightType)
	{
	case IRender_Light::POINT:
		LightPoint->SetLightColor(FLinearColor(C.r, C.g, C.b, C.a));
		break;
	case IRender_Light::SPOT:
		SpotPoint->SetLightColor(FLinearColor(C.r, C.g, C.b, C.a));
		break;
	}

}

void AStalkerLight::set_color(float r, float g, float b)
{
	LightPoint->SetLightColor(FLinearColor(r,g, b, 1.0f));
}

void AStalkerLight::set_hud_mode(bool b)
{
}

bool AStalkerLight::get_hud_mode()
{
	return false;
}

// Called every frame
void AStalkerLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

