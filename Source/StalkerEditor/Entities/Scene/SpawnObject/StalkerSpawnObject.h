#pragma once
#include "Kernel/StalkerEngineManager.h"
#include "StalkerSpawnObject.generated.h"

UCLASS()
class  AStalkerSpawnObject : public AActor
{
	GENERATED_BODY()
public:
										AStalkerSpawnObject			(const FObjectInitializer& ObjectInitializer);
	

	void								PostActorCreated			() override;
	void								BeginDestroy				() override;
	void								PostEditImport				() override;
	FString								GetDefaultActorLabel		() const override;

	void								Attach						(class USceneComponent* InComponent);
	void								Detach						(class USceneComponent* InComponent);

	bool								ShouldTickIfViewportsOnly	() const override;

	void								Tick						(float DeltaSeconds) override;
	void								Serialize					(FArchive& Ar) override;

	void								DestroyEntity				();
	void								CreateEntity				();
	void								SpawnWrite					();
	void								SpawnRead					();
	void								Destroyed					() override;
	bool								Modify						(bool bAlwaysMarkDirty = true) override;
	void								PostEditUndo				() override;

	UPROPERTY(Transient)
	FString								DisplayName;

	UPROPERTY(VisibleAnywhere)
	FString								SectionName;

	UPROPERTY(VisibleAnywhere)
	EStalkerGame						GameType;

	UPROPERTY(SkipSerialization)
	TArray<uint8>						EntityData;
	ISE_Abstract*						XRayEntity;
	UPROPERTY(EditAnywhere)
	bool								ExcludeFromBuild = false;

protected:
	void								BeginPlay					() override;

private:
	void								CreateSpawnData				();
	void								CreateVisualData			();
	void								CreateInventoryItemData		();
	void								CreateNCPData				();

	UPROPERTY(EditInstanceOnly,TextExportTransient)
	class USceneComponent*				SceneComponent;

	UPROPERTY()
	class UStalkerSpawnObjectGraphPointComponent *GraphPointComponent;

	UPROPERTY()
	class UStalkerKinematicsComponent*		MainVisual;

	UPROPERTY()
	TArray<class UStalkerKinematicsComponent*>Visuals;
	
	UPROPERTY(NonTransactional)
	class UPointLightComponent*			PointLight;
	
	UPROPERTY(NonTransactional)
	class USpotLightComponent*			SpotLight;

	UPROPERTY(EditAnywhere, EditFixedSize, NoClear, Transient, TextExportTransient)
	class UStalkerSpawnProperties_Base*	SpawnData;
	UPROPERTY(EditAnywhere, EditFixedSize, NoClear, Transient, TextExportTransient)
	class UStalkerSpawnProperties_Visual*VisualData;
	UPROPERTY(EditAnywhere, EditFixedSize, NoClear, Transient, TextExportTransient)
	class UStalkerSpawnProperties_ALifeInventoryItem* InventoryItemData;
	UPROPERTY(EditAnywhere, EditFixedSize, NoClear, Transient, TextExportTransient)
	class UStalkerSpawnProperties_ALifeTraderAbstract* NCPData;
	UPROPERTY()
	UBillboardComponent*				SpawnBillboard;
	UPROPERTY()
	UBillboardComponent*				SpawnBillboardBad;

	class CLAItem*						LightAnim;
	
	UPROPERTY()
	TObjectPtr<class UArrowComponent>	ArrowComponent;
};
