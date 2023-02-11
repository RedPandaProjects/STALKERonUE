#include "StalkerAIMapEditorRenderComponent.h"
#include "Kernel/Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/AIMap/StalkerAIMap.h"
#include "../../../UI/EdMode/AIMap/StalkerAIMapEditMode.h"
TCustomShowFlag<> StalkerShowAIMap(TEXT("StalkerShowAIMap"), false /*DefaultEnabled*/, SFG_Normal, FText::FromString(TEXT("AI Map")));

UStalkerAIMapEditorRenderComponent::UStalkerAIMapEditorRenderComponent()
{
	bAutoActivate = true;
	bTickInEditor = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.5;
	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	bUseEditorCompositing = true;
	SetGenerateOverlapEvents(false);
}

FPrimitiveSceneProxy* UStalkerAIMapEditorRenderComponent::CreateSceneProxy()
{
	class FStalkerAIMapEditorRenderSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}
		FStalkerAIMapEditorRenderSceneProxy(const UStalkerAIMapEditorRenderComponent& InComponent)
			: FPrimitiveSceneProxy(&InComponent)
		{

			//ViewFlagName = TEXT("Navigation");
			ViewFlagIndex = uint32(FEngineShowFlags::FindIndexByName(TEXT("StalkerShowAIMap")));

		}

		FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{  
			const bool bVisibleMe = View->Family->EngineShowFlags.GetSingleFlag(ViewFlagIndex) && !GLevelEditorModeTools().GetActiveMode(FStalkerAIMapEditMode::EM_AIMap);
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
			AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
			if (!StalkerWorldSettings)
			{
				return;
			}
			UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
			if (!StalkerAIMap)
			{
				return;
			}

			check(StalkerWorldSettings->EditorMaterialAIMap);

			int32 CountNode = 0;
			auto NodeToUV = [](int32 Node)
			{
				return FVector2f(static_cast<float>(Node % 4) * 0.25f, static_cast<float>(Node / 4) * 0.25f);
			};

			const float NodeSize = (StalkerAIMap->NodeSize * 0.9f) * 0.5f;

			auto DrawNode = [this, NodeToUV, StalkerWorldSettings, NodeSize, &CountNode](FDynamicMeshBuilder& MeshBuilder, FPlane4f& NodePlane, int32 Node, float x, float y, float z, bool IsSelected)
			{
				const FVector3f PlaneNormalRender = FVector3f(0, 0, 1);
				FVector3f Vertex1 = FMath::RayPlaneIntersection(FVector3f(x - NodeSize, y - NodeSize, z), PlaneNormalRender, NodePlane);
				FVector3f Vertex2 = FMath::RayPlaneIntersection(FVector3f(x - NodeSize, y + NodeSize, z), PlaneNormalRender, NodePlane);
				FVector3f Vertex3 = FMath::RayPlaneIntersection(FVector3f(x + NodeSize, y - NodeSize, z), PlaneNormalRender, NodePlane);
				FVector3f Vertex4 = FMath::RayPlaneIntersection(FVector3f(x + NodeSize, y + NodeSize, z), PlaneNormalRender, NodePlane);
				Vertex1.Z += 4.f;
				Vertex2.Z += 4.f;
				Vertex3.Z += 4.f;
				Vertex4.Z += 4.f;
				MeshBuilder.AddVertex(FDynamicMeshVertex(Vertex1, FVector3f(1, 0, 0), FVector3f(0, 1, 0), FVector2f(0, 0) + NodeToUV(Node), IsSelected ? FColor::White : FColor(127, 127, 127, 255)));
				MeshBuilder.AddVertex(FDynamicMeshVertex(Vertex2, FVector3f(1, 0, 0), FVector3f(0, 1, 0), FVector2f(0, 0.25f) + NodeToUV(Node), IsSelected ? FColor::White : FColor(127, 127, 127, 255)));
				MeshBuilder.AddVertex(FDynamicMeshVertex(Vertex3, FVector3f(1, 0, 0), FVector3f(0, 1, 0), FVector2f(0.25f, 0) + NodeToUV(Node), IsSelected ? FColor::White : FColor(127, 127, 127, 255)));
				MeshBuilder.AddVertex(FDynamicMeshVertex(Vertex4, FVector3f(1, 0, 0), FVector3f(0, 1, 0), FVector2f(0.25f, 0.25f) + NodeToUV(Node), IsSelected ? FColor::White : FColor(127, 127, 127, 255)));
				MeshBuilder.AddTriangle(0 + CountNode * 4, 1 + CountNode * 4, 2 + CountNode * 4);
				MeshBuilder.AddTriangle(1 + CountNode * 4, 3 + CountNode * 4, 2 + CountNode * 4);
				CountNode++;
			};
			const double DistanceRenderAIMap = (StalkerWorldSettings->DistanceRenderAIMap) * 0.5f;


			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					FDynamicMeshBuilder MeshBuilder(GetScene().GetFeatureLevel());
					MeshBuilder.ReserveTriangles(70 * 70 * 2);
					MeshBuilder.ReserveVertices(70 * 70 * 4);
					FMeshBatch Mesh;
					for (FStalkerAIMapNode* Node : StalkerAIMap->Nodes)
					{
						int32 NodeLink = 0;
						if (Node->NodeLeft)
						{
							NodeLink |= 1 << 2;
						}
						if (Node->NodeForward)
						{
							NodeLink |= 1 << 3;
						}
						if (Node->NodeRight)
						{
							NodeLink |= 1 << 0;
						}
						if (Node->NodeBackward)
						{
							NodeLink |= 1 << 1;
						}
						if (FVector::DistXY(View->ViewLocation, FVector(Node->Position)) < DistanceRenderAIMap * 100.0)
						{
							DrawNode(MeshBuilder, Node->Plane, NodeLink, Node->Position.X, Node->Position.Y, Node->Position.Z, EnumHasAnyFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected));
						}

					}
					MeshBuilder.GetMesh(FMatrix::Identity, StalkerWorldSettings->EditorMaterialAIMap->GetRenderProxy(), SDPG_World, true, false, ViewIndex, Collector);
				}
			}

		}
		int32 ViewFlagIndex;
	};

	return new FStalkerAIMapEditorRenderSceneProxy(*this);
}

void UStalkerAIMapEditorRenderComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials /*= false*/) const
{

	Super::GetUsedMaterials(OutMaterials, false);
	if (bGetDebugMaterials)
	{
		UWorld* World = GetWorld();
		AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
		if (!StalkerWorldSettings)
		{
			return;
		}
		OutMaterials.Add(StalkerWorldSettings->EditorMaterialAIMap);
	}
}

void UStalkerAIMapEditorRenderComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	MarkRenderStateDirty();
}

FBoxSphereBounds UStalkerAIMapEditorRenderComponent::CalcBounds(const FTransform& LocalToWorld) const
{

	FBox BBox(ForceInit);

	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		const FVector BoxExtent(1.f);
		return FBoxSphereBounds(LocalToWorld.GetLocation(), BoxExtent, 1.f);
	}
	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (StalkerAIMap)
	{
		BBox = FBox(StalkerAIMap->AABB);
	}

	if (BBox.IsValid)
	{
		// Points are in world space, so no need to transform.
		return FBoxSphereBounds(BBox);
	}
	else
	{
		const FVector BoxExtent(1.f);
		return FBoxSphereBounds(LocalToWorld.GetLocation(), BoxExtent, 1.f);
	}
}
