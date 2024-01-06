#include "StalkerEnvironment.h"


AStalkerEnvironment::AStalkerEnvironment()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AStalkerEnvironment::SetWeather(const char* Name, bool Force)
{
}

bool AStalkerEnvironment::SetEffect(const char* Name)
{
	return false;
}

void AStalkerEnvironment::SetGameTime(float InTime, float InTimeFactor)
{
}

void AStalkerEnvironment::SetWindStrengthFactor(float InFactor)
{
}

bool AStalkerEnvironment::IsEffectPlaying() const
{
	return false;
}

float AStalkerEnvironment::GetGameTime() const
{
	return 0;
}

float AStalkerEnvironment::GetRainDensity() const
{
	return 0;
}

shared_str AStalkerEnvironment::GetWeather() const
{
	return "none";
}

float AStalkerEnvironment::GetFogDensity() const
{
	return 0;
}

float AStalkerEnvironment::GetFogPlane() const
{
	return 1000;
}

float AStalkerEnvironment::GetWindStrengthFactor() const
{
	return 0;
}
