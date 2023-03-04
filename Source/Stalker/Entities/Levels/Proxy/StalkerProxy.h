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
	void							Lock						(class CObject* XRayObject) override;
	void							Unlock						(class CObject* XRayObject) override;
	
	void							Attach						(class IRenderVisual* Visual, const char* BoneName) override;
	void							AttachAsRoot				(class IRenderVisual* Visual) override;
	void							Detach						(class IRenderVisual* Visual) override;
	AStalkerProxy*					CastToStalkerProxy			() override;
	class AStalkerPlayerCharacter*	CastToStalkerPlayerCharacter() override;
protected:
	virtual void					BeginPlay					() override;
	virtual void					Tick						(float DeltaTime) override;
private:
	class CObject*					XRayObject;


};
