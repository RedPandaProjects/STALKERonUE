#pragma once
#include "StalkerEditorRenderProxy.generated.h"
UCLASS(NotBlueprintable,notplaceable)
class AStalkerEditorRenderProxy : public AActor
{
	GENERATED_BODY()
public:
	AStalkerEditorRenderProxy();
private:
	UPROPERTY(VisibleAnywhere)
	class UStalkerAIMapEditorRenderComponent* AIMapEditorRenderComponent;
};
