#include "StalkerGameInstance.h"
#include "../../StalkerEngineManager.h"

void UStalkerGameInstance::Init()
{
	Super::Init();
}

void UStalkerGameInstance::Shutdown()
{
	GStalkerEngineManager->DetachViewport(GetGameViewportClient());
	Super::Shutdown();
}

void UStalkerGameInstance::OnStart()
{
	Super::OnStart();
	GStalkerEngineManager->AttachViewport(GetGameViewportClient());
}
