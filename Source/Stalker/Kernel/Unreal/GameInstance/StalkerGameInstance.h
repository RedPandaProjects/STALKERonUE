#pragma once
#include "StalkerGameInstance.generated.h"

UCLASS()
class STALKER_API UStalkerGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	void Init() override;
	void Shutdown() override;

private:
protected:
	void OnStart() override;

};
