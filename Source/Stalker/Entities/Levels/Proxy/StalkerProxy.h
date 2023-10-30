#pragma once

THIRD_PARTY_INCLUDES_START
#include "XrEngine/XRayUnrealProxyInterface.h"
THIRD_PARTY_INCLUDES_END

#include "StalkerProxy.generated.h"

UCLASS(Blueprintable)
class STALKER_API AStalkerProxy : public AActor,public XRayUnrealProxyInterface
{
	GENERATED_BODY()
	
public:	
									AStalkerProxy				();
	void							BeginDestroy				() override;
	void							Lock						(CObject*XRayObject) override;
	void							Lock						(void* XRayObject) override;
	void							Unlock						(void* XRayObject) override;
	
	void							SetAsRoot					(class XRayUnrealAttachableInterface* Visual) override;
	
	void							AttachTo					(class XRayUnrealAttachableInterface* AttachableInterface, const char* BoneName) override;
	void							Detach						() override;
	void							SetOffset					(const Fmatrix&offset) override;
	void*							CastUnrealObject			(EXRayUnrealObjectType ObjectType) override;
	void							SetOwnerNoSee				(bool Enable) override;
	void							SetOnlyOwnerSee				(bool Enable) override;
	void							GetWorldTransform			(Fmatrix& OutXForm) override;
	bool							IsAttached					(XRayUnrealAttachableInterface* Attach) override;
	void							SetVisibility				(bool NewVisibility) override;
protected:
	virtual void					BeginPlay					() override;
	virtual void					Tick						(float DeltaTime) override;

public:

private:
	class CObject*					XRayObject;


};
