#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StalkerDecal.generated.h"

UCLASS()
class STALKER_API AStalkerDecal : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AStalkerDecal();

	void SetDecalData(const FVector &Location, float Size, const FVector &Normal, UMaterialInstanceDynamic *Material);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UDecalComponent* Decal;
};
