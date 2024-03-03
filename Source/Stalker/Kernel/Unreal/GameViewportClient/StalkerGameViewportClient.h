// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/GameViewportClient.h"
#include "StalkerGameViewportClient.generated.h"
UCLASS()
class UStalkerGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
	
public:
	void Activated		(FViewport* InViewport, const FWindowActivateEvent& InActivateEvent) override;
	void Deactivated	(FViewport* InViewport, const FWindowActivateEvent& InActivateEvent) override;
	bool InputKey		(const FInputKeyEventArgs& EventArgs) override;
	bool InputAxis		(FViewport* Viewport, FInputDeviceId InputDevice, FKey Key, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false) override;
	void Tick			(float DeltaTime) override;
	void LostFocus		(FViewport* Viewport) override;
	void ReceivedFocus	(FViewport* Viewport) override;
	void BeginDestroy	() override;
	
	UPROPERTY(Transient)
	bool IsActive = false;

};
