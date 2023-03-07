#include "StalkerSpawnObjectSphereShapeComponent.h"
#include "../StalkerSpawnObject.h"
#include "Kernel/Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/Spawn/StalkerLevelSpawn.h"
#include "UObject/GCObjectScopeGuard.h"

FPrimitiveSceneProxy* UStalkerSpawnObjectSphereShapeComponent::CreateSceneProxy()
{
	class FStalkerSpawnObjectSphereShapeComponent final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}
		FStalkerSpawnObjectSphereShapeComponent(const UStalkerSpawnObjectSphereShapeComponent& InComponent)
			: FPrimitiveSceneProxy(&InComponent), SphereShapeComponent(&InComponent)
		{
			ViewFlagIndex = uint32(FEngineShowFlags::FindIndexByName(TEXT("StalkerShowSpawnShape")));
		}

		FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bVisibleMe = View->Family->EngineShowFlags.GetSingleFlag(ViewFlagIndex);;
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
			if (const UStalkerSpawnObjectSphereShapeComponent*InSphereShapeComponent = SphereShapeComponent)
			{
				UWorld* World = GetScene().GetWorld();
				for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
				{
					if (VisibilityMap & (1 << ViewIndex))
					{
						const FSceneView* View = Views[ViewIndex];
						FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
						FVector Position = GetLocalToWorld().GetOrigin();
						FColor Color = InSphereShapeComponent->ShapeColor;
						if (IsSelected())
						{
							Color.A = Color.A * 2;
						}
						const FMaterialRenderProxy* const ColoredMeshInstance = &Collector.AllocateOneFrameResource<FColoredMaterialRenderProxy>(GEngine->DebugMeshMaterial->GetRenderProxy(), Color);
						GetSphereMesh(Position, GetBounds().GetBox().GetExtent(), 10, 7, ColoredMeshInstance, SDPG_World, false, ViewIndex, Collector, false, new HActor(InSphereShapeComponent->GetOwner(), InSphereShapeComponent));
					}
				}
			}
		}
		int32 ViewFlagIndex;
		const UStalkerSpawnObjectSphereShapeComponent* SphereShapeComponent;

	};

	return new FStalkerSpawnObjectSphereShapeComponent(*this);
}

FBoxSphereBounds UStalkerSpawnObjectSphereShapeComponent::CalcBounds(const FTransform& InLocalToWorld) const
{
	FSphere BSphere(ForceInit);
	BSphere.Center = InLocalToWorld.GetLocation();
	BSphere.W = InLocalToWorld.GetScale3D().X * SphereRadius;
	return FBoxSphereBounds(BSphere);
}

void UStalkerSpawnObjectSphereShapeComponent::UpdateColor()
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

void UStalkerSpawnObjectSphereShapeComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	UpdateColor();
}

bool UStalkerSpawnObjectSphereShapeComponent::Modify(bool bAlwaysMarkDirty /*= true*/)
{
	bool bResult = Super::Modify(bAlwaysMarkDirty);

	if (!IsValid(GetWorld()) || GetWorld()->IsGameWorld()||!bAlwaysMarkDirty)
	{
		return bResult;
	}
	if (AActor* Actor = GetAttachParentActor())
	{
		if (!Actor->IsA<AStalkerSpawnObject>())
		{
			return bResult;
		}
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(GetWorld()->GetWorldSettings());
	if (IsValid(StalkerWorldSettings))
	{
		UStalkerLevelSpawn* Spawn = StalkerWorldSettings->GetSpawn();
		if (IsValid(Spawn))
		{
			StalkerWorldSettings->Modify();
			StalkerWorldSettings->NeedRebuildSpawn = true;
			Spawn->NeedRebuild = true;
			Spawn->Modify();
		}
	}
	return bResult;
}
