// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "StalkerWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class AStalkerWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString AlifeName;
	UPROPERTY(Transient)
	class AStalkerDebugRender* DebugRender;
};
