#include "StalkerSoundSource.h"

#include "AudioDevice.h"
#include "StalkerSoundManager.h"
#include "Kernel/StalkerEngineManager.h"

void UStalkerSoundSource::Play(CObject* ActorSource, bool Looping, float Delay)
{
	Stop();
	USoundBase *InSound =Looping ? LoopingSound : Sound;
	if (!InSound || !GEngine || !GEngine->UseSound())
	{
		return;
	}

	UWorld* ThisWorld = GWorld;
	if (!ThisWorld || !ThisWorld->bAllowAudioPlayback || ThisWorld->IsNetMode(NM_DedicatedServer))
	{
		return;
	}

	AActor* WorldContextOwner = nullptr;
	if(ActorSource&&ActorSource->UnrealProxy)
	{
		WorldContextOwner = reinterpret_cast<AActor*>(ActorSource->UnrealProxy->CastUnrealObject(ERBMKUnrealObjectType::Actor));
	}

	const bool bPersistAcrossLevelTransition = WorldContextOwner == nullptr;
	const FAudioDevice::FCreateComponentParams Params = bPersistAcrossLevelTransition
	 ? FAudioDevice::FCreateComponentParams(ThisWorld->GetAudioDeviceRaw())
	 : FAudioDevice::FCreateComponentParams(ThisWorld, WorldContextOwner);


	AudioComponent = FAudioDevice::CreateComponent(InSound, Params);
	if (ensure(AudioComponent))
	{
		AudioComponent->SetVolumeMultiplier(1);
		AudioComponent->SetPitchMultiplier(1);
		AudioComponent->bAllowSpatialization = false;
		AudioComponent->bIsUISound = true;
		AudioComponent->bAutoDestroy = !Looping;
		AudioComponent->bIgnoreForFlushing = bPersistAcrossLevelTransition;
		AudioComponent->SubtitlePriority = InSound->GetSubtitlePriority();
		AudioComponent->bStopWhenOwnerDestroyed = false;
		
		if(FMath::IsNearlyZero(Delay)||Delay<0)
		{
			AudioComponent->Play();
		}
		else
		{
			FTimerDelegate Delegate; 
			Delegate.BindUObject(this, &UStalkerSoundSource::DelayPlay); 
			GWorld->GetTimerManager().SetTimer(TimerHandle_DelayPlay, Delegate, Delay, false);
		}
	}
	PausedCounter = 0;
	if(ActorSource)
	{
		UStalkerSoundManager*SoundManager = CastChecked<UStalkerSoundManager>(GetOuter());
		SoundManager->RBMKObject2SoundSource.Add(ActorSource,this);
		RBMKSource = ActorSource;
		TimeToPropagade = FMath::RandRange(0.5f-0.3f,0.5f+0.3f);
	}
}


void UStalkerSoundSource::Play(CObject* ActorSource, const Fvector& InPosition, bool Looping, float Delay)
{
	Stop();
	USoundBase *InSound =Looping ? LoopingSound : Sound;
	if (!InSound || !GEngine || !GEngine->UseSound())
	{
		return;
	}

	UWorld* ThisWorld = GWorld;
	if (!ThisWorld || !ThisWorld->bAllowAudioPlayback || ThisWorld->IsNetMode(NM_DedicatedServer))
	{
		return ;
	}

	const bool bIsInGameWorld = ThisWorld->IsGameWorld();

	AActor* WorldContextOwner = nullptr;
	if(ActorSource&&ActorSource->UnrealProxy)
	{
		WorldContextOwner = reinterpret_cast<AActor*>(ActorSource->UnrealProxy->CastUnrealObject(ERBMKUnrealObjectType::Actor));
	}

	FAudioDevice::FCreateComponentParams Params(ThisWorld, WorldContextOwner);
	Params.SetLocation(FVector(StalkerMath::RBMKLocationToUnreal(InPosition)));
	AudioComponent = FAudioDevice::CreateComponent(InSound, Params);

	if (AudioComponent)
	{
		AudioComponent->SetWorldLocation(FVector(StalkerMath::RBMKLocationToUnreal(InPosition)));
		AudioComponent->SetVolumeMultiplier(1);
		AudioComponent->SetPitchMultiplier(1);
		AudioComponent->bAllowSpatialization	= Params.ShouldUseAttenuation();
		AudioComponent->bIsUISound				= !bIsInGameWorld;
		AudioComponent->bAutoDestroy			= true;
		AudioComponent->SubtitlePriority		= InSound->GetSubtitlePriority();
		AudioComponent->bStopWhenOwnerDestroyed = false;
		if(FMath::IsNearlyZero(Delay)||Delay<0)
		{
			AudioComponent->Play();
		}
		else
		{
			FTimerDelegate Delegate; 
			Delegate.BindUObject(this, &UStalkerSoundSource::DelayPlay); 
			GWorld->GetTimerManager().SetTimer(TimerHandle_DelayPlay, Delegate, Delay, false);
		}
	}

	PausedCounter = 0;
	if(ActorSource)
	{
		UStalkerSoundManager*SoundManager = CastChecked<UStalkerSoundManager>(GetOuter());
		SoundManager->RBMKObject2SoundSource.Add(ActorSource,this);
		RBMKSource = ActorSource;
		TimeToPropagade = FMath::RandRange(0.5f-0.3f,0.5f+0.3f);
	}
}


void UStalkerSoundSource::PlayWithoutFeedback(CObject* ActorSource, bool Looping, float Delay, float* Volume)
{
	if(ActorSource)
	{
		UStalkerSoundManager*SoundManager = CastChecked<UStalkerSoundManager>(GetOuter());
		UStalkerSoundSource*StalkerSoundSource = NewObject<UStalkerSoundSource>(SoundManager);
		StalkerSoundSource->RBMKName = RBMKName;
		StalkerSoundSource->Sound = Sound;
		StalkerSoundSource->LoopingSound = LoopingSound;
		StalkerSoundSource->Flags = Flags;
		StalkerSoundSource->AIRatioToDistance = AIRatioToDistance;
		SoundManager->SoundSourcesWithFeedback.Add(StalkerSoundSource);
		StalkerSoundSource->Play(ActorSource,Looping,Delay);
		if(Volume)
		{
			StalkerSoundSource->SetVolume(*Volume);
		}
		return;
	}
	USoundBase *InSound =Looping ? LoopingSound : Sound;
	if (!InSound || !GEngine || !GEngine->UseSound())
	{
		return;
	}

	UWorld* ThisWorld = GWorld;
	if (!ThisWorld || !ThisWorld->bAllowAudioPlayback || ThisWorld->IsNetMode(NM_DedicatedServer))
	{
		return;
	}

	AActor* WorldContextOwner = nullptr;
	if(ActorSource&&ActorSource->UnrealProxy)
	{
		WorldContextOwner = reinterpret_cast<AActor*>(ActorSource->UnrealProxy->CastUnrealObject(ERBMKUnrealObjectType::Actor));
	}

	const bool bPersistAcrossLevelTransition = WorldContextOwner == nullptr;
	const FAudioDevice::FCreateComponentParams Params = bPersistAcrossLevelTransition
	 ? FAudioDevice::FCreateComponentParams(ThisWorld->GetAudioDeviceRaw())
	 : FAudioDevice::FCreateComponentParams(ThisWorld, WorldContextOwner);


	UAudioComponent* InAudioComponent = FAudioDevice::CreateComponent(InSound, Params);
	if (ensure(InAudioComponent))
	{
		if(Volume)
		{
			InAudioComponent->SetVolumeMultiplier(*Volume);
		}
		InAudioComponent->SetPitchMultiplier(1);
		InAudioComponent->bAllowSpatialization = false;
		InAudioComponent->bIsUISound = true;
		InAudioComponent->bAutoDestroy = !Looping;
		InAudioComponent->bIgnoreForFlushing = bPersistAcrossLevelTransition;
		InAudioComponent->SubtitlePriority = InSound->GetSubtitlePriority();
		InAudioComponent->bStopWhenOwnerDestroyed = false;
		
		if(FMath::IsNearlyZero(Delay)||Delay<0)
		{
			InAudioComponent->Play();
		}
		else
		{
			FTimerDelegate Delegate; 
			Delegate.BindUObject(this, &UStalkerSoundSource::DelayPlay); 
			GWorld->GetTimerManager().SetTimer(TimerHandle_DelayPlay, Delegate, Delay, false);
		}
	}
	PausedCounter = 0;
}

void UStalkerSoundSource::PlayWithoutFeedback(CObject* ActorSource, const Fvector& InPosition, bool Looping,float Delay, float* Volume)
{
	if(ActorSource)
	{
		UStalkerSoundManager*SoundManager = CastChecked<UStalkerSoundManager>(GetOuter());
		UStalkerSoundSource*StalkerSoundSource = NewObject<UStalkerSoundSource>(SoundManager);
		StalkerSoundSource->RBMKName = RBMKName;
		StalkerSoundSource->Sound = Sound;
		StalkerSoundSource->LoopingSound = LoopingSound;
		StalkerSoundSource->Flags = Flags;
		StalkerSoundSource->AIRatioToDistance = AIRatioToDistance;
		SoundManager->SoundSourcesWithFeedback.Add(StalkerSoundSource);
		StalkerSoundSource->Play(ActorSource,InPosition,Looping,Delay);
		if(Volume)
		{
			StalkerSoundSource->SetVolume(*Volume);
		}
		return;
	}
	USoundBase *InSound =Looping ? LoopingSound : Sound;
	if (!InSound || !GEngine || !GEngine->UseSound())
	{
		return;
	}

	UWorld* ThisWorld = GWorld;
	if (!ThisWorld || !ThisWorld->bAllowAudioPlayback || ThisWorld->IsNetMode(NM_DedicatedServer))
	{
		return ;
	}

	const bool bIsInGameWorld = ThisWorld->IsGameWorld();

	AActor* WorldContextOwner = nullptr;
	if(ActorSource&&ActorSource->UnrealProxy)
	{
		WorldContextOwner = reinterpret_cast<AActor*>(ActorSource->UnrealProxy->CastUnrealObject(ERBMKUnrealObjectType::Actor));
	}

	FAudioDevice::FCreateComponentParams Params(ThisWorld, WorldContextOwner);
	Params.SetLocation(FVector(StalkerMath::RBMKLocationToUnreal(InPosition)));
	UAudioComponent* InAudioComponent = FAudioDevice::CreateComponent(InSound, Params);

	if (InAudioComponent)
	{
		InAudioComponent->SetWorldLocation(FVector(StalkerMath::RBMKLocationToUnreal(InPosition)));
		if(Volume)
		{
			InAudioComponent->SetVolumeMultiplier(*Volume);
		}
		InAudioComponent->SetPitchMultiplier(1);
		InAudioComponent->bAllowSpatialization	= Params.ShouldUseAttenuation();
		InAudioComponent->bIsUISound				= !bIsInGameWorld;
		InAudioComponent->bAutoDestroy			= true;
		InAudioComponent->SubtitlePriority		= InSound->GetSubtitlePriority();
		InAudioComponent->bStopWhenOwnerDestroyed = false;
		if(FMath::IsNearlyZero(Delay)||Delay<0)
		{
			InAudioComponent->Play();
		}
		else
		{
			FTimerDelegate Delegate; 
			Delegate.BindUObject(this, &UStalkerSoundSource::DelayPlay); 
			GWorld->GetTimerManager().SetTimer(TimerHandle_DelayPlay, Delegate, Delay, false);
		}
	}

	PausedCounter = 0;
}

void UStalkerSoundSource::Stop()
{
	if(IsValid(AudioComponent))
	{
		AudioComponent->Stop();
		AudioComponent->DestroyComponent();
		AudioComponent->MarkAsGarbage();
		AudioComponent = nullptr;
	}
}

void UStalkerSoundSource::StopWithFade()
{
	if(IsValid(AudioComponent))
	{
		AudioComponent->FadeOut(0.1f,1);
		Stop();
	}
}

void UStalkerSoundSource::Pause(bool Paused)
{
	if(IsValid(AudioComponent))
	{
		if(!ensure(PausedCounter>=0))
		{
			PausedCounter = 0;
		}
		if(!Paused)
		{
			PausedCounter--;
		}
		if(PausedCounter == 0)
		{
			if(TimerHandle_DelayPlay.IsValid())
			{
				if(Paused)
				{
					GWorld->GetTimerManager().PauseTimer(TimerHandle_DelayPlay);
				}
				else
				{
					GWorld->GetTimerManager().UnPauseTimer(TimerHandle_DelayPlay);
				}
			}
			else
			{
				AudioComponent->SetPaused(Paused);
			}
		}
		if(Paused)
		{
			PausedCounter++;
		}
	}

}

void UStalkerSoundSource::SetVolume(float NewVolume)
{
	if(IsValid(AudioComponent))
	{
		AudioComponent->SetVolumeMultiplier(NewVolume);
	}
}

void UStalkerSoundSource::SetFrequency(float NewFrequency)
{
	if(IsValid(AudioComponent))
	{
		AudioComponent->SetPitchMultiplier(NewFrequency);
	}
}

void UStalkerSoundSource::SetPosition(const Fvector& InPosition)
{
	if(IsValid(AudioComponent))
	{
		AudioComponent->SetWorldLocation(FVector( StalkerMath::RBMKLocationToUnreal(InPosition)));
	}
}

void UStalkerSoundSource::SetRange(float MinDistance, float MaxDistance)
{
	if(IsValid(AudioComponent))
	{
		if(!AudioComponent->bOverrideAttenuation)
		{
			if(const FSoundAttenuationSettings* SoundAttenuationSettings =  AudioComponent->GetAttenuationSettingsToApply())
			{
				AudioComponent->AttenuationOverrides = *SoundAttenuationSettings;
			}
			AudioComponent->bOverrideAttenuation = true;
		}
		AudioComponent->AttenuationOverrides.FalloffDistance = MaxDistance*100.f - MinDistance*100.f;
		AudioComponent->AttenuationOverrides.AttenuationShapeExtents.X = MinDistance*100.f;
	}
}

bool UStalkerSoundSource::IsPlaying() const
{
	if(IsValid(AudioComponent))
	{
		if(TimerHandle_DelayPlay.IsValid())
		{
			return GWorld->GetTimerManager().IsTimerActive(TimerHandle_DelayPlay);
		}
		return AudioComponent->IsPlaying();
	}
	return false;
}

bool UStalkerSoundSource::IsRelative()
{
	if(IsValid(AudioComponent))
	{
		return AudioComponent->bIsUISound;
	}
	return false;
}

float UStalkerSoundSource::GetDuration() const
{
	if(IsValid(AudioComponent))
	{
		return Sound->GetDuration();
	}
	return 0;
}

Fvector UStalkerSoundSource::GetWorldPosition() const
{
	if(IsValid(AudioComponent))
	{
		return StalkerMath::UnrealLocationToRBMK(AudioComponent->GetComponentLocation());
	}
	return Fvector(0,0,0);
}

float UStalkerSoundSource::GetMinDistance() const
{
	if(IsValid(AudioComponent))
	{
		if(const FSoundAttenuationSettings* SoundAttenuationSettings =  AudioComponent->GetAttenuationSettingsToApply())
		{
			return SoundAttenuationSettings->AttenuationShapeExtents.X/100.f;
		}
	}
	return 0;
}

float UStalkerSoundSource::GetMaxDistance() const
{
	if(IsValid(AudioComponent))
	{
		if(const FSoundAttenuationSettings* SoundAttenuationSettings =  AudioComponent->GetAttenuationSettingsToApply())
		{
			return (SoundAttenuationSettings->FalloffDistance+ SoundAttenuationSettings->AttenuationShapeExtents.X)/100.f;
		}
	}
	return 0;
}

float UStalkerSoundSource::GetVolume() const
{
	if(IsValid(AudioComponent))
	{
		return AudioComponent->VolumeMultiplier;
	}
	return 1;
}

shared_str UStalkerSoundSource::GetName() const
{
	return RBMKName;
}

void UStalkerSoundSource::DelayPlay()
{
	if(IsValid(AudioComponent))
	{
		return AudioComponent->Play();
	}
	GWorld->GetTimerManager().ClearTimer(TimerHandle_DelayPlay);
}
