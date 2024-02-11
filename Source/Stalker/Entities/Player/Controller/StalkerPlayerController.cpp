#include "StalkerPlayerController.h"

#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"
#include "..\..\..\Kernel\Unreal\UI\StalkerRBMKUIWidget.h"

void AStalkerPlayerController::CreateHUD()
{
	if(!IsLocalPlayerController())
	{
		return;
	}
	if (UIWidget)
	{
		return;
	}
	UIWidget = CreateWidget<UStalkerRBMKUIWidget>(this, UStalkerRBMKUIWidget::StaticClass());
	UIWidget->AddToViewport();
}

void AStalkerPlayerController::BeginPlay()
{
	Super::BeginPlay();
	const UStalkerGameSettings*StalkerGameSettings = GetDefault<UStalkerGameSettings>();
	USoundMix* ShockEffectorMix = StalkerGameSettings->ShockEffectorMix.LoadSynchronous();
	if(!ensure(ShockEffectorMix))
	{
		return;
	}
	UGameplayStatics::PushSoundMixModifier(GetWorld(),ShockEffectorMix);
}
