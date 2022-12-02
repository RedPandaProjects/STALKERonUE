#include "StalkerGameInstance.h"
#include "../../StalkerEngineManager.h"

void UStalkerGameInstance::Init()
{
	Super::Init();
}

void UStalkerGameInstance::Shutdown()
{
	GXRayEngineManager->DetachViewport(GetGameViewportClient());
	Super::Shutdown();
}

void UStalkerGameInstance::OnStart()
{
	Super::OnStart();
	GXRayEngineManager->AttachViewport(GetGameViewportClient());
}
