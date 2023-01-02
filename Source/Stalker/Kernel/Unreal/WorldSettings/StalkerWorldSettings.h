#pragma once
#include "StalkerWorldSettings.generated.h"

UCLASS()
class STALKER_API AStalkerWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString						AlifeName;
	UPROPERTY(Transient)
	class AStalkerDebugRender*	DebugRender;
	
	class UStalkerCForm*		GetOrCreateCForm	();
	class UStalkerCForm*		GetCForm			();
private:
	UPROPERTY()
	class UStalkerCForm* CForm;
};
