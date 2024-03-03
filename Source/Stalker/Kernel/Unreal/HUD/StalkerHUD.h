#pragma once
#include "StalkerHUD.generated.h"

UCLASS()
class STALKER_API AStalkerHUD : public AHUD
{
	GENERATED_BODY()

public:
			AStalkerHUD		();
	void	DrawHUD			() override;
private:
	UPROPERTY(Transient)
	uint32 LastFrame = 0;
};
