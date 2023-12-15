#pragma once
#include "NiagaraActor.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/Render/ParticleCustom.h"
THIRD_PARTY_INCLUDES_END
#include "StalkerNiagaraActor.generated.h"

UCLASS()
class STALKER_API AStalkerNiagaraActor : public ANiagaraActor, public IRenderVisual,public IParticleCustom
{
	GENERATED_BODY()
	
public:
								AStalkerNiagaraActor		(const FObjectInitializer& ObjectInitializer);
	void						Initialize					(shared_str Name);

	void						AttachTo					(XRayUnrealAttachableInterface* Attach, const char* BoneName) override;
	void						Detach						() override;
	void						Lock						(CObject*) override;
	void						Lock						(void*) override;
	void						Unlock						(void*) override;

	void*						CastUnrealObject			(EXRayUnrealObjectType ObjectType) override;
	void*						QueryInterface				(EXRayUnrealInterfaceType AttachableType) override;

	void						BeginDestroy				() override;
	vis_data&					getVisData					() override;
	shared_str					getDebugName				() override;
	void						UpdateParent				(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM) override;
	void						Play						() override;
	void						Stop						(BOOL bDefferedStop = TRUE) override;
	BOOL						IsPlaying					() override;
	BOOL						IsLooping					() override;
	const shared_str			Name						() override;
	void						SetHudMode					(BOOL b) override;
	BOOL						GetHudMode					() override;
	bool						Alive						() override;
	void						SetOffset					(const Fmatrix&offset) override;
	void						SetEnableVelocity			(bool EnableVelocity) override ;
	
	void						BeginPlay					() override;
	void						Tick						(float DeltaSeconds) override;
	void						SetOwnerNoSee				(bool Enable) override;
	void						SetOnlyOwnerSee				(bool Enable) override;
	void						GetWorldTransform			(Fmatrix& OutXForm) override;
	bool						IsAttached					(XRayUnrealAttachableInterface* Attach) override;
	void						SetVisibility				(bool NewVisibility) override;

	shared_str					ParticlesName;

public:
	UPROPERTY(Transient)
	bool						HudMode = false;
	UPROPERTY(Transient)
	class UStalkerNiagaraSystem*StalkerNiagaraSystem;
private:
	UFUNCTION()
	void						OnSystemFinished			(class UNiagaraComponent* PSystem);

public:
	

private:
	void*						Locked = nullptr;
	vis_data					VisData;

	bool						IsLive  = true;
	FVector						LastPosition;
};
