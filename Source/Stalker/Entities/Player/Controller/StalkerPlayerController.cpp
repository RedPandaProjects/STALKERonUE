#include "StalkerPlayerController.h"
#include "Kernel/Unreal/UI/StalkerUIWidget.h"

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
	UIWidget = CreateWidget<UStalkerUIWidget>(this, UStalkerUIWidget::StaticClass());
	UIWidget->AddToViewport();
}
