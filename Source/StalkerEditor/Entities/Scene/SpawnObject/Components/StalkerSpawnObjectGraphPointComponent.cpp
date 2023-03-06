#include "StalkerSpawnObjectGraphPointComponent.h"
#include "../StalkerSpawnObject.h"
#include "UObject/GCObjectScopeGuard.h"

FPrimitiveSceneProxy* UStalkerSpawnObjectGraphPointComponent::CreateSceneProxy()
{
	class FStalkerSpawnObjectGraphPointComponent final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}
		FStalkerSpawnObjectGraphPointComponent(const UStalkerSpawnObjectGraphPointComponent& InComponent)
			: FPrimitiveSceneProxy(&InComponent), GraphPointComponent(&InComponent)
		{
		}

		FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bVisibleMe = true;
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
					FVector Position = GetLocalToWorld().GetOrigin();
					FColor Color = FColor::Green;
					if (const UStalkerSpawnObjectGraphPointComponent*InGraphPointComponent = GraphPointComponent)
					{
						Color = InGraphPointComponent->GraphPointColor;
						if (IsSelected())
						{
							Color.A = Color.A * 2;
						}
					}
				

					const FMaterialRenderProxy* const ColoredMeshInstance = &Collector.AllocateOneFrameResource<FColoredMaterialRenderProxy>(GEngine->DebugMeshMaterial->GetRenderProxy(), Color);
					TArray<FDynamicMeshVertex> MeshVerts;
					TArray<uint32> MeshIndices;

					{
						static const u32 Indices[12] = { 1,3,0, 3,2,0, 2,4,0, 4,1,0 };
						static FVector Points[5] = 
						{
							{0.0f ,	0.0f ,	1.0f},
							{0.0f ,	-0.5f,	0.0f },
							{0.0f ,	0.5f ,	0.0f},
							{-0.5f,	0.0f ,	0.0f},
							{0.5f ,	0.0f ,	0.0f},
						};
						for (int32 FaceID = 0;FaceID<_countof(Indices)/3; FaceID++)
						{
							FVector Vertex1 = Points[Indices[FaceID*3+0]];
							FVector Vertex2 = Points[Indices[FaceID * 3 + 1]];
							FVector Vertex3 = Points[Indices[FaceID * 3 + 2]];
							{
								FDynamicMeshVertex Vertex;
								Vertex.TangentX = ((Vertex2 - Vertex1) ^ (Vertex3 - Vertex1)).GetSafeNormal();
								Vertex.TangentZ = (Vertex.TangentX.ToFVector() ^ FVector(0,1,0)).GetSafeNormal();
								Vertex.Color = Color;
								Vertex.Position = FVector3f(Vertex1)*100.f;
								MeshIndices.Add(MeshVerts.Add(Vertex));
								Vertex.Position = FVector3f(Vertex2) * 100.f;
								MeshIndices.Add(MeshVerts.Add(Vertex));
								Vertex.Position = FVector3f(Vertex3) * 100.f;
								MeshIndices.Add(MeshVerts.Add(Vertex));
							}
						}

					}

					FDynamicMeshBuilder MeshBuilder(Collector.GetFeatureLevel());
					MeshBuilder.AddVertices(MeshVerts);
					MeshBuilder.AddTriangles(MeshIndices);
					MeshBuilder.GetMesh(GetLocalToWorld(), ColoredMeshInstance, SDPG_World, false, false, true, ViewIndex, Collector, nullptr);
				}
			}

		}
		const UStalkerSpawnObjectGraphPointComponent* GraphPointComponent;
	};

	return new FStalkerSpawnObjectGraphPointComponent(*this);
}

FBoxSphereBounds UStalkerSpawnObjectGraphPointComponent::CalcBounds(const FTransform& InLocalToWorld) const
{
	FSphere BSphere(ForceInit);
	BSphere.Center = InLocalToWorld.GetLocation();
	BSphere.W = 100.f;
	return FBoxSphereBounds(BSphere);
}

void UStalkerSpawnObjectGraphPointComponent::UpdateColor()
{
	AStalkerSpawnObject*SpawnObject = Cast<AStalkerSpawnObject>(GetOwner());
	GraphPointColor = FColor(0x60ff00ff);/*
	if (SpawnObject && SpawnObject->SectionName.Len())
	{
		if (pSettings->line_exist(TCHAR_TO_ANSI(*SpawnObject->SectionName), "shape_transp_color"))
		{
			ShapeColor = FColor(pSettings->r_color(TCHAR_TO_ANSI(*SpawnObject->SectionName), "shape_transp_color"));
		}
	}*/
}

void UStalkerSpawnObjectGraphPointComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	UpdateColor();
}
