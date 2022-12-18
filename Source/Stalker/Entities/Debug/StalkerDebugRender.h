#pragma once
#include "StalkerDebugRender.generated.h"

UCLASS()
class STALKER_API AStalkerDebugRender : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStalkerDebugRender();

	UPROPERTY(EditAnywhere)
	class ULineBatchComponent* LineBatchComponent;

	UPROPERTY(EditAnywhere)
	int32	RenderDistance = 150;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AddTriangle(const  FVector& InP1, const  FVector& InP2, const FVector& InP3, const FColor& Color);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	u32	CurrentFrame;

	TMap<FColor,TArray<FVector>> Faces;
};
