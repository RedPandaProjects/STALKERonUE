#pragma once
#include "Debug/DebugDrawComponent.h"
#include "StalkerAIMapEditorRenderComponent.generated.h"
UCLASS(NotBlueprintable, notplaceable)
class UStalkerAIMapEditorRenderComponent : public UPrimitiveComponent
{
	GENERATED_BODY()
public:
	UStalkerAIMapEditorRenderComponent();

	FPrimitiveSceneProxy* CreateSceneProxy()  override;
	void GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials) const override;


	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

};
