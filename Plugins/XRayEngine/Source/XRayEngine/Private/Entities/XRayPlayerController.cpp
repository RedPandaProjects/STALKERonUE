// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/XRayPlayerController.h"
#include "Render/XRayUIWidget.h"

void AXRayPlayerController::CreateHUD()
{
	if(!IsLocalPlayerController())
	{
		return;
	}
	if (UIWidget)
	{
		return;
	}
	UIWidget = CreateWidget<UXRayUIWidget>(this, UXRayUIWidget::StaticClass());
	UIWidget->AddToViewport();
}
