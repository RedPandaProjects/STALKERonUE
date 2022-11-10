// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "XRayPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AXRayPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void CreateHUD();
private:
	UPROPERTY(Transient)
	TObjectPtr<class UXRayUIWidget> UIWidget;
};
