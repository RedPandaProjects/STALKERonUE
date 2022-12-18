// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "StalkerGameMode.generated.h"
/**
 * 
 */
UCLASS()
class STALKER_API AStalkerGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AStalkerGameMode(const FObjectInitializer& ObjectInitializer);
	void Tick(float DeltaSeconds) override;
	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	void StartPlay() override;
	void ResetLevel() override;
	void Reset() override;
};
