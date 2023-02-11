#include "StalkerSpawnObjectBoxShapeComponent.h"
#include "../StalkerSpawnObject.h"

FPrimitiveSceneProxy* UStalkerSpawnObjectBoxShapeComponent::CreateSceneProxy()
{
	class FStalkerSpawnObjectBoxShapeComponent final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}
		FStalkerSpawnObjectBoxShapeComponent(const UStalkerSpawnObjectBoxShapeComponent& InComponent)
			: FPrimitiveSceneProxy(&InComponent), SphereShapeComponent(&InComponent)
		{
			ViewFlagIndex = uint32(FEngineShowFlags::FindIndexByName(TEXT("StalkerShowSpawnShape")));
		}

		FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bVisibleMe = View->Family->EngineShowFlags.GetSingleFlag(ViewFlagIndex);
			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = bVisibleMe && IsShown(View);
			Result.bDynamicRelevance = true;
			// ideally the TranslucencyRelevance should be filled out by the material, here we do it conservative
			Result.bSeparateTranslucency = Result.bNormalTranslucency = bVisibleMe;
			return Result;
		}
		uint32 GetMemoryFootprint(void) const override { return sizeof * this + GetAllocatedSize(); }
		uint32 GetAllocatedSize(void) const { return (uint32)FPrimitiveSceneProxy::GetAllocatedSize(); }

		void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, class FMeshElementCollector& Collector) const override
		{
			UWorld* World = GetScene().GetWorld();
			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					FColor Color = SphereShapeComponent->ShapeColor;
					if (IsSelected())
					{
						Color.A = Color.A * 2;
					}
					const FMaterialRenderProxy* const ColoredMeshInstance = &Collector.AllocateOneFrameResource<FColoredMaterialRenderProxy>(GEngine->DebugMeshMaterial->GetRenderProxy(), Color);
					GetBoxMesh(GetLocalToWorld(), SphereShapeComponent->BoxExtent, ColoredMeshInstance, SDPG_World, ViewIndex, Collector,new HActor(SphereShapeComponent->GetOwner(), SphereShapeComponent.Get()));
				}
			}

		}
		int32 ViewFlagIndex;
		TWeakObjectPtr<const UStalkerSpawnObjectBoxShapeComponent> SphereShapeComponent;
	};

	return new FStalkerSpawnObjectBoxShapeComponent(*this);
}

FBoxSphereBounds UStalkerSpawnObjectBoxShapeComponent::CalcBounds(const FTransform& InLocalToWorld) const
{
	return FBoxSphereBounds(FBox(-BoxExtent, BoxExtent)).TransformBy(InLocalToWorld);
}

void UStalkerSpawnObjectBoxShapeComponent::UpdateColor()
{
	AStalkerSpawnObject* SpawnObject = Cast<AStalkerSpawnObject>(GetOwner());
	ShapeColor = FColor(0x3C808080);
	if (SpawnObject && SpawnObject->SectionName.Len())
	{
		if (pSettings->line_exist(TCHAR_TO_ANSI(*SpawnObject->SectionName), "shape_transp_color"))
		{
			ShapeColor = FColor(pSettings->r_color(TCHAR_TO_ANSI(*SpawnObject->SectionName), "shape_transp_color"));
		}
	}
}

void UStalkerSpawnObjectBoxShapeComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	UpdateColor();
}
