#pragma once
#include "StalkerSoundWaveAssetUserData.h"

#include "StalkerSoundSource.generated.h"
UCLASS()
class STALKER_API UStalkerSoundSource : public UObject,public IRBMKSoundSource
{
	GENERATED_BODY() 
public:
	void		Play							(CObject*ActorSource, bool Looping = false, float Delay = 0.f) override;\
	void		Play							(CObject*ActorSource, const Fvector&InPosition, bool Looping = false, float Delay = 0.f) override;\

	void		PlayWithoutFeedback				(CObject*ActorSource, bool Looping = false, float Delay = 0.f,float* Volume = nullptr) override;
	void		PlayWithoutFeedback				(CObject*ActorSource, const Fvector&InPosition, bool Looping = false, float Delay = 0.f,float* Volume = nullptr) override;\

	void		Stop							() override;
	void		StopWithFade					() override;
	void		Pause							(bool Paused,int32 PausedID) ;
	void		SetVolume						(float NewVolume) override;
	void		SetFrequency					(float NewFrequency) override;
	void		SetPosition						(const Fvector&InPosition) override;
	void		SetRange						(float MinDistance,float MaxDistance) override;

	bool		IsPlaying						() const override;
	bool		IsRelative						() override;
	float		GetDuration						() const override;
	Fvector		GetWorldPosition				() const override;
	float		GetMinDistance					() const override;
	float		GetMaxDistance					() const override;
	float		GetVolume						() const override;
	shared_str	GetName							() const override;
	
	CObject*									RBMKSource = nullptr;
	float										TimeToPropagade = 0.5f;
		
	UFUNCTION()
	void DelayPlay();
	
	shared_str	RBMKName;
	
	UPROPERTY(VisibleAnywhere,Transient)
	USoundBase*	Sound;
	
	UPROPERTY(VisibleAnywhere,Transient)
	USoundBase*	LoopingSound;

	UPROPERTY(VisibleAnywhere,Transient, meta=(ClampMin=0, ClampMax=1))
	float AIRatioToDistance = 1;

	UPROPERTY(VisibleAnywhere,Transient,Meta = (Bitmask, BitmaskEnum = "/Script/Stalker.EStalkerSoundTypes"))
	int32 Flags = 0;
	
	UPROPERTY(VisibleAnywhere,Transient)
	UAudioComponent* AudioComponent;
	
	UPROPERTY(VisibleAnywhere,Transient)
	int32	PausedID = 0;
	
	UPROPERTY(Transient)
	FTimerHandle TimerHandle_DelayPlay;

	UPROPERTY(Transient)
	int32 ReferenceCounter = 1;

};
