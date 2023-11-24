#pragma once
#include "StalkerWayObject.generated.h"

UCLASS()
class  AStalkerWayObject : public AActor
{
	GENERATED_BODY()
public:
												AStalkerWayObject			(const FObjectInitializer& ObjectInitializer);
	class UStalkerWayPointComponent*			CreatePoint					(const FVector&Position,bool AutoLink);
	void										RemoveSelect				();
	void										InvertLinkSelect			();
	void										RemoveLinkSelect			();
	void										Convert1LinkSelect			();
	void										Convert2LinkSelect			();
	void										Add1LinkSelect				();
	void										Add2LinkSelect				();
	bool										CheckName					(const FString&Name);
	void										CalculateIndex				();
	void										Destroyed					() override;
	bool										Modify						(bool bAlwaysMarkDirty = true) override;
	void										PostEditUndo				() override;

	UPROPERTY(EditInstanceOnly, EditFixedSize, NoClear, Category = "WayObject")
	TArray<class UStalkerWayPointComponent*>	Points;



private:
	void										GetSelectedPoint			(TArray<class UStalkerWayPointComponent*>& SelectedPoints);
	UPROPERTY()
	class USceneComponent*						SceneComponent;
	UPROPERTY()
	UBillboardComponent *						Billboard;

};
