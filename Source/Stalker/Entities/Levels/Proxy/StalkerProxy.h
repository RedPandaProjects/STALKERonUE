#pragma once

THIRD_PARTY_INCLUDES_START
#include "XrEngine/XRayUnrealProxyInterface.h"
THIRD_PARTY_INCLUDES_END

#include "StalkerProxy.generated.h"

UCLASS()
class STALKER_API AStalkerProxy : public AActor,public XRayUnrealProxyInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStalkerProxy();

	void Initilize(class CObject* XRayObject);
	void Attach(class IRenderVisual* Visual) override;
	void Detach(class IRenderVisual* Visual) override;
	void DetachAll();
	class AStalkerProxy* CastToStalkerProxy() override;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
private:
	UPROPERTY(EditAnywhere,Transient)
	TSet<USceneComponent*> XRayComponents;
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;
	class CObject* XRayObject;

};
