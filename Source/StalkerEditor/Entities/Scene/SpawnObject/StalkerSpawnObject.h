#pragma once
#include "StalkerSpawnObject.generated.h"

UCLASS()
class  AStalkerSpawnObject : public AActor
{
	GENERATED_BODY()
public:
										AStalkerSpawnObject			(const FObjectInitializer& ObjectInitializer);
	

	void								PostActorCreated			() override;
	void								BeginDestroy				() override;
	void								PostInitProperties			() override;

	void								Attach						(class USceneComponent* InComponent);
	void								Detach						(class USceneComponent* InComponent);

	bool								ShouldTickIfViewportsOnly	() const override;

	void								Tick						(float DeltaSeconds) override;
	void								Serialize					(FArchive& Ar) override;

	void								DestroyEntity				();
	void								CreateEntity				();
	void								SpawnWrite					();
	void								SpawnRead					();

	UPROPERTY(Transient)
	FString								DisplayName;

	UPROPERTY(VisibleAnywhere)
	FString								SectionName;


	TArray<uint8>						EntityData;



private:
	void								CreateSpawnData				();
	void								CreateVisualData			();
	void								CreateInventoryItemData		();
	void								CreateNCPData				();

	UPROPERTY(EditInstanceOnly)
	class USceneComponent*				SceneComponent;

	UPROPERTY(Transient)
	TSet<USceneComponent*>				XRayComponents;

	UPROPERTY(Transient)
	class UStalkerKinematicsComponent*	MainVisual;

	UPROPERTY(Transient)
	TArray<class UStalkerKinematicsComponent*>  Visuals;
	
	UPROPERTY(Transient)
	class UPointLightComponent*			PointLight;
	
	UPROPERTY(Transient)
	class USpotLightComponent*			SpotLight;

	UPROPERTY(EditAnywhere, EditFixedSize, NoClear, Transient)
	class UStalkerSpawnProperties_Base*	SpawnData;
	UPROPERTY(EditAnywhere, EditFixedSize, NoClear, Transient)
	class UStalkerSpawnProperties_Visual*VisualData;
	UPROPERTY(EditAnywhere, EditFixedSize, NoClear, Transient)
	class UStalkerSpawnProperties_ALifeInventoryItem* InventoryItemData;
	UPROPERTY(EditAnywhere, EditFixedSize, NoClear, Transient)
	class UStalkerSpawnProperties_ALifeTraderAbstract* NCPData;
	UPROPERTY()
	UBillboardComponent*				SpawnBillboard;

	ISE_Abstract*						XRayEntity;
	class CLAItem*						LightAnim;




};
