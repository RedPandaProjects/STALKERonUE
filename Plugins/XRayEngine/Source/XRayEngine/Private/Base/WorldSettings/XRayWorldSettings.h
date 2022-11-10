// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "XRayWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class AXRayWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString AlifeName;
};
