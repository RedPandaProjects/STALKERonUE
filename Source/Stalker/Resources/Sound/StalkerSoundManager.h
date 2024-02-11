#pragma once
#include "StalkerSoundSource.h"
#include "StalkerSoundManager.generated.h"
UCLASS()
class STALKER_API UStalkerSoundManager : public UObject,public IRBMKSoundManager
{
	GENERATED_BODY()
public:
	IRBMKSoundSource*					CreateSource			(const char* Name) override;
	IRBMKSoundSource*					CreateSource			(const char* Name, s32 CustomFlags) override;
	IRBMKSoundSource*					CreateSource			(const TSoftObjectPtr<USoundBase>& SoftSoundPtr, s32 CustomFlags = -1);
	void								Destroy					(IRBMKSoundSource* SoundSource) override;
	IRBMKSoundSource*					AddReference			(IRBMKSoundSource*SoundSource) override;
	IRBMKSoundSource*					Duplicate				(IRBMKSoundSource* SoundSource) override;
	IRBMKSoundSource*					Duplicate				(IRBMKSoundSource* SoundSource, s32 CustomFlags) override;
	void 								Destroyed				(IRBMKSoundActorListener* SoundActorListener) override;
	void 								Destroyed				(CObject* Object) override;
	void								SetShockEffectorVolume	(float NewVolume) override;
	bool								ExistSoundWave			(const char* Name) const override;
	void								Tick					(float DeltaTime);
	void								Build					();
	void								Clear					();
	int32								PauseEmitters			(bool Paused);
	UPROPERTY(Transient)	
	TSet<UStalkerSoundSource*>			SoundSources;
	
	UPROPERTY(Transient)	
	TSet<UStalkerSoundSource*>			SoundSourcesWithFeedback;
	
	TMap<CObject*,UStalkerSoundSource*>	RBMKObject2SoundSource;

	UPROPERTY(Transient)
	TSet<FString>						SoundsList;			
	
	xr_vector<ISpatial*>				TempSpatial;

	UPROPERTY(Transient)
	int32								PauseCounter = -1;
};