#pragma once

#include "StalkerProxy.generated.h"

UCLASS(Blueprintable)
class STALKER_API AStalkerProxy : public AActor,public IRBMKUnrealProxy
{
	GENERATED_BODY()
	
public:	
									AStalkerProxy				();
	void							BeginDestroy				() override;
	void							Lock						(CObject*XRayObject) override;
	void							Lock						(void* XRayObject) override;
	void							Unlock						(void* XRayObject) override;
	
	void							SetAsRoot					(class IRBMKUnrealAttachable* Visual) override;
	void							AttachTo					(class IRBMKUnrealAttachable* AttachableInterface, const char* BoneName) override;
	void							Detach						() override;
	void							SetOffset					(const Fmatrix&offset) override;
	void*							CastUnrealObject			(ERBMKUnrealObjectType ObjectType) override;
	void							SetOwnerNoSee				(bool Enable) override;
	void							SetOnlyOwnerSee				(bool Enable) override;
	void							GetWorldTransform			(Fmatrix& OutXForm) override;
	bool							IsAttached					(IRBMKUnrealAttachable* Attach) override;
	void							SetVisibility				(bool NewVisibility) override;
	UFUNCTION(BlueprintCallable,Category="Stalker")
	bool							IsStalker();
	UFUNCTION(BlueprintImplementableEvent,Category="Stalker",meta=(DisplayName="PostSetAsRoot"))
	void							K2_PostSetAsRoot();
protected:
	virtual void					BeginPlay					() override;
	virtual void					Tick						(float DeltaTime) override;

public:

private:
	class CObject*					XRayObject;


};
