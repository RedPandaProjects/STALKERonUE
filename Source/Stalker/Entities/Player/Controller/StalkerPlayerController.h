#pragma once

#include "StalkerPlayerController.generated.h"
UCLASS()
class AStalkerPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	void BeginPlay() override;
};
