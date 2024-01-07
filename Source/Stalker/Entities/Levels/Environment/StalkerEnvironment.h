#pragma once
#include "StalkerEnvironment.generated.h"

UCLASS()
class STALKER_API AStalkerEnvironment : public AActor,public IRBMKEnvironment
{
	GENERATED_BODY()

public:
				AStalkerEnvironment		();
	bool		SetEffect				(const char* Name) override;
public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetGameTime(float InTime, float InTimeFactor) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	bool IsEffectPlaying() const override;
	
	UFUNCTION(BlueprintImplementableEvent)
	float GetGameTime() const override;
	
	UFUNCTION(BlueprintImplementableEvent)
	float GetRainDensity() const override;
	
	UFUNCTION(BlueprintImplementableEvent)
	float GetFogDensity() const override;
	
	UFUNCTION(BlueprintImplementableEvent)
	float GetFogPlane() const override;
protected:
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "SetEffect"))
	bool K2_SetEffect(const FString&Name);
	
};
