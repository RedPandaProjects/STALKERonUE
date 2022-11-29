#pragma once
#include "XRayGameInstance.generated.h"

UCLASS()
class XRAYENGINE_API UXRayGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	void Init() override;
	void Shutdown() override;

private:
protected:
	void OnStart() override;

};
