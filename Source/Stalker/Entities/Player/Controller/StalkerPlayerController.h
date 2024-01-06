#pragma once

#include "StalkerPlayerController.generated.h"
UCLASS()
class AStalkerPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void CreateHUD();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<class UStalkerUIWidget> UIWidget;
};
