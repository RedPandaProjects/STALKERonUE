#include "StalkerPlayerController.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"

void AStalkerPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if(IsLocalPlayerController())
	{
		const UStalkerGameSettings*StalkerGameSettings = GetDefault<UStalkerGameSettings>();
		USoundMix* ShockEffectorMix = StalkerGameSettings->ShockEffectorMix.LoadSynchronous();
		if(!ensure(ShockEffectorMix))
		{
			return;
		}
		UGameplayStatics::PushSoundMixModifier(GetWorld(),ShockEffectorMix);
	}
}
