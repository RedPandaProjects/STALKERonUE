#pragma once
#include "StalkerEnvironment.generated.h"

UCLASS()
class STALKER_API AStalkerEnvironment : public AActor,public IRBMKEnvironment
{
	GENERATED_BODY()

public:
				AStalkerEnvironment		();
	void		SetWeather				(const char* Name, bool Force) override;
	bool		SetEffect				(const char* Name) override;
	void		SetGameTime				(float InTime, float InTimeFactor) override;
	void		SetWindStrengthFactor	(float InFactor) override;
	bool		IsEffectPlaying			() const override;
	float		GetGameTime				() const override;
	float		GetRainDensity			() const override;
	shared_str	GetWeather				() const override;
	float		GetFogDensity			() const override;
	float		GetFogPlane				() const override;
	float		GetWindStrengthFactor	() const override;
};
