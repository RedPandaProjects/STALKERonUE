#include "StalkerEditorRenderProxy.h"
#include "Components/StalkerAIMapEditorRenderComponent.h"

AStalkerEditorRenderProxy::AStalkerEditorRenderProxy()
{	
	bIsEditorOnlyActor = true;
	AIMapEditorRenderComponent = CreateDefaultSubobject< UStalkerAIMapEditorRenderComponent>("AIMapEditorRenderComponent");
	SetRootComponent(AIMapEditorRenderComponent);

}

