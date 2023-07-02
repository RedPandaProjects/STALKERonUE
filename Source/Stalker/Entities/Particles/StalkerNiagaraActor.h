#pragma once
#include "NiagaraActor.h"
THIRD_PARTY_INCLUDES_START
#include "XrRender/Public/ParticleCustom.h"
THIRD_PARTY_INCLUDES_END
#include "StalkerNiagaraActor.generated.h"

UCLASS()
class STALKER_API AStalkerNiagaraActor : public ANiagaraActor, public IRenderVisual,public IParticleCustom
{
	GENERATED_BODY()
	
public:
								AStalkerNiagaraActor		(const FObjectInitializer& ObjectInitializer);
	void						Lock						();
	void						Unlock						();
	void						BeginDestroy				() override;
	void						Initialize					(shared_str Name);
	vis_data&					getVisData					() override;
	u32							getType						() override;
	shared_str					getDebugName				() override;
	IParticleCustom*			dcast_ParticleCustom		() override;
	void						Detach						() override;
	void						UpdateParent				(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM) override;
	void						Play						() override;
	void						Stop						(BOOL bDefferedStop = TRUE) override;
	BOOL						IsPlaying					() override;
	BOOL						IsLooping					() override;
	const shared_str			Name						() override;
	void						SetHudMode					(BOOL b) override;
	BOOL						GetHudMode					() override;
	bool						Alive						() override;
	shared_str					ParticlesName;
	UPROPERTY(Transient)
	bool						HudMode = false;
	UPROPERTY(Transient)
	class UStalkerNiagaraSystem*StalkerNiagaraSystem;
private:
	UFUNCTION()
	void						OnSystemFinished			(class UNiagaraComponent* PSystem);

	bool						IsLocked = false;
	vis_data					VisData;

	bool						IsLive  = true;
};
