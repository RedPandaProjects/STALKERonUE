#include "StalkerEditorCForm.h"
#include "Kernel/Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/CFrom/StalkerCForm.h"
#include "ToolMenus.h"
#include "../../UI/Commands/StalkerEditorCommands.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterialsManager.h"
#include "Kernel/StalkerEngineManager.h"
#include "Components/BrushComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterial.h"
#include "LandscapeDataAccess.h"
#include "LandscapeProxy.h"
#include "LandscapeComponent.h"
#include "PhysicalMaterials/PhysicalMaterialMask.h"
#include "../../StalkerEditorManager.h"

void UStalkerEditorCForm::Initialize()
{
	GStalkerEditorManager->UICommandList->MapAction(StalkerEditorCommands::Get().BuildCForm,FExecuteAction::CreateUObject(this, &UStalkerEditorCForm::Build));
}

void UStalkerEditorCForm::Destroy()
{
	PhysicalMaterial2ID.Empty();
}

void UStalkerEditorCForm::Build()
{
	if (FApp::IsGame())
	{
		return;
	}
	GStalkerEngineManager->GetPhysicalMaterialsManager()->Build();
	if (!GStalkerEngineManager->GetPhysicalMaterialsManager()->DefaultPhysicalMaterial)
	{
		UE_LOG(LogStalkerEditor,Error,TEXT("Stalker physical materials is empty"));
		return;
	}
	

	FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	if (!WorldContext)
		return;

	UWorld* World = WorldContext->World();
	if (!IsValid(World))
		return;

	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}
	if (StalkerWorldSettings->NotForXRay)
	{
		return;
	}
	UStalkerCForm*CForm = StalkerWorldSettings->GetOrCreateCForm();
	CForm->InvalidCForm();

	for (int32 Index = 0; Index < GStalkerEngineManager->GetPhysicalMaterialsManager()->PhysicalMaterials.Num(); Index++)
	{
		PhysicalMaterial2ID.FindOrAdd(GStalkerEngineManager->GetPhysicalMaterialsManager()->PhysicalMaterials[Index]) = Index;
		CForm->Name2ID.Add(GStalkerEngineManager->GetPhysicalMaterialsManager()->Names[Index].c_str());
	}
	int32 DefaultID = GStalkerEngineManager->GetPhysicalMaterialsManager()->PhysicalMaterials.IndexOfByKey(GStalkerEngineManager->GetPhysicalMaterialsManager()->DefaultPhysicalMaterial);
	check(DefaultID != INDEX_NONE);

	const int32 TerrainExportLOD = 0;
	for (auto& WorldVertex : World->GetModel()->Points)
	{
		CForm->Vertices.Add(WorldVertex);
		CForm->AABB += WorldVertex;
	}
	
	for (auto& WorldNode : World->GetModel()->Nodes)
	{
		if (WorldNode.NumVertices <= 2)
		{
			continue;
		}

		int32 Index0 = World->GetModel()->Verts[WorldNode.iVertPool + 0].pVertex;
		int32 Index1 = World->GetModel()->Verts[WorldNode.iVertPool + 1].pVertex;
		int32 Index2;
	
		for (auto v = 2; v < WorldNode.NumVertices; ++v)
		{
			Index2 = World->GetModel()->Verts[WorldNode.iVertPool + v].pVertex;

			UMaterialInterface*Material = World->GetModel()->Surfs[WorldNode.iSurf].Material;
			UPhysicalMaterial*PhysMaterial = Material? Material->GetPhysicalMaterial():nullptr;
			int32* IndexMaterial = PhysicalMaterial2ID.Find(Cast<UStalkerPhysicalMaterial>(PhysMaterial));
			FStalkerCFormTriangle Triangle;
			Triangle.MaterialIndex = IndexMaterial? static_cast<uint32>(*IndexMaterial): DefaultID;
			Triangle.VertexIndex0 = Index0;
			Triangle.VertexIndex1 = Index1;
			Triangle.VertexIndex2 = Index2;
			CForm->Triangles.Add(Triangle);
			Index1 = Index2;
		}
	}
	FTriMeshCollisionData MeshColisionData = {};
	TArray<UStaticMeshComponent*>StaticMeshComponents;
	for (TActorIterator<AActor> AactorItr(World); AactorItr; ++AactorItr)
	{
		
		AactorItr->GetComponents<UStaticMeshComponent>(StaticMeshComponents, true);
		for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
		{
			if (StaticMeshComponent->Mobility != EComponentMobility::Movable&&
				StaticMeshComponent->GetStaticMesh()&&
				StaticMeshComponent->GetStaticMesh()->HasValidRenderData()
				)
			{

				FStaticMeshLODResources& LODModel = StaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[0];
				FIndexArrayView Indices = LODModel.IndexBuffer.GetArrayView();
				MeshColisionData.UVs.Reset();
				MeshColisionData.Vertices.Reset();
				MeshColisionData.Indices.Reset();
				MeshColisionData.MaterialIndices.Reset();
				StaticMeshComponent->GetStaticMesh()->GetPhysicsTriMeshData(&MeshColisionData, false);

			
				//UInstancedStaticMeshComponent
			
				auto BuildStaticMesh = [this,&MeshColisionData,StaticMeshComponent,CForm,DefaultID](const FTransform&Transform)
				{
					int32 StartVertexIdx = CForm->Vertices.Num();
					for (FVector3f& InVertex : MeshColisionData.Vertices)
					{
						FVector3f Vertex = FVector3f(Transform.TransformPosition(FVector(InVertex)));
						CForm->AABB +=	Vertex;
						CForm->Vertices.Add(Vertex);
					}

					for (int32 i = 0; i < MeshColisionData.Indices.Num(); ++i)
					{
						UMaterialInterface* Material = StaticMeshComponent->GetMaterial(MeshColisionData.MaterialIndices[i]);

						UPhysicalMaterial* PhysMaterial = Material ? Material->GetPhysicalMaterial() : nullptr;
						int32* IndexMaterial = PhysicalMaterial2ID.Find(Cast<UStalkerPhysicalMaterial>(PhysMaterial));
						FStalkerCFormTriangle Triangle;
						Triangle.MaterialIndex = IndexMaterial ? static_cast<uint32>(*IndexMaterial) : DefaultID;

						Triangle.VertexIndex0 = StartVertexIdx + MeshColisionData.Indices[i].v0;
						Triangle.VertexIndex1 = StartVertexIdx + MeshColisionData.Indices[i].v2;
						Triangle.VertexIndex2 = StartVertexIdx + MeshColisionData.Indices[i].v1;
						CForm->Triangles.Add(Triangle);

					}
				};
				if (UInstancedStaticMeshComponent*InstancedStaticMeshComponent = Cast<UInstancedStaticMeshComponent>(StaticMeshComponent))
				{
					for (int32 i = 0; i < InstancedStaticMeshComponent->GetInstanceCount(); i++)
					{	
						FTransform WorldSpaceInstanceTransform;
						InstancedStaticMeshComponent->GetInstanceTransform(i,WorldSpaceInstanceTransform,true);
						BuildStaticMesh(WorldSpaceInstanceTransform);
					}
				}
				else
				{
					BuildStaticMesh(StaticMeshComponent->GetComponentTransform());
				}
			
			}

		}
		ALandscapeProxy* LandscapeProxy = Cast<ALandscapeProxy>(*AactorItr);
		if (LandscapeProxy)
		{
			const int32 ComponentSizeQuads = ((LandscapeProxy->ComponentSizeQuads + 1) >> TerrainExportLOD) - 1;
			const float ScaleFactor = (float)LandscapeProxy->ComponentSizeQuads / (float)ComponentSizeQuads;

			int32 MinX = MAX_int32, MinY = MAX_int32;
			int32 MaxX = MIN_int32, MaxY = MIN_int32;

			// Find range of entire landscape
			for (int32 ComponentIndex = 0; ComponentIndex < LandscapeProxy->LandscapeComponents.Num(); ComponentIndex++)
			{
				ULandscapeComponent* Component = LandscapeProxy->LandscapeComponents[ComponentIndex];
								Component->GetComponentExtent(MinX, MinY, MaxX, MaxY);
			}
			const FVector2D UVScale = FVector2D(1.0f, 1.0f) / FVector2D((MaxX - MinX) + 1, (MaxY - MinY) + 1);

			TArray<uint32> MaskData;
			int32 MaskSizeX = 0, MaskSizeY = 0, MaskAdressX =0, MaskAdressY= 0;
			bool UseMask = false;
			if (IsValid(LandscapeProxy->GetLandscapeMaterial())&& IsValid(LandscapeProxy->GetLandscapeMaterial()->GetPhysicalMaterialMask()))
			{
				LandscapeProxy->GetLandscapeMaterial()->GetPhysicalMaterialMask()->GenerateMaskData(MaskData, MaskSizeX, MaskSizeY);
				UseMask = MaskSizeX* MaskSizeY>0;
				MaskAdressX = LandscapeProxy->GetLandscapeMaterial()->GetPhysicalMaterialMask()->AddressX;
				MaskAdressY = LandscapeProxy->GetLandscapeMaterial()->GetPhysicalMaterialMask()->AddressY;
			}
		

			for (ULandscapeComponent* LandscapeComponent : LandscapeProxy->LandscapeComponents)
			{
				FLandscapeComponentDataInterface CDI(LandscapeComponent, TerrainExportLOD);

				for (auto y = 0; y < LandscapeComponent->ComponentSizeQuads; ++y)
				{
					for (auto x = 0; x < LandscapeComponent->ComponentSizeQuads; ++x)
					{
						auto StartIndex = CForm->Vertices.Num();
						CForm->Vertices.Add(FVector3f(CDI.GetWorldVertex(x, y)));
						CForm->Vertices.Add(FVector3f(CDI.GetWorldVertex(x, y + 1)));
						CForm->Vertices.Add(FVector3f(CDI.GetWorldVertex(x + 1, y + 1)));
						CForm->Vertices.Add(FVector3f(CDI.GetWorldVertex(x + 1, y)));


						FVector2D TextureUV0 = FVector2D(x * ScaleFactor + LandscapeComponent->GetSectionBase().X, y * ScaleFactor + LandscapeComponent->GetSectionBase().Y);
						FVector2D TextureUV1 = FVector2D(x * ScaleFactor + LandscapeComponent->GetSectionBase().X, (y+1) * ScaleFactor + LandscapeComponent->GetSectionBase().Y);
						FVector2D TextureUV2 = FVector2D((x+1) * ScaleFactor + LandscapeComponent->GetSectionBase().X, (y + 1) * ScaleFactor + LandscapeComponent->GetSectionBase().Y);
						FVector2D TextureUV3 = FVector2D((x + 1) * ScaleFactor + LandscapeComponent->GetSectionBase().X, y * ScaleFactor + LandscapeComponent->GetSectionBase().Y);



						FVector2D TextureUV_T0 = (((TextureUV0 + TextureUV2 + TextureUV3) / 3.f) - FVector2D(MinX, MinY)) * UVScale;


						FStalkerCFormTriangle Triangle;

						UPhysicalMaterial* PhysicalMaterial = nullptr;
						if (UseMask)
						{
							PhysicalMaterial =	LandscapeProxy->GetLandscapeMaterial()->GetPhysicalMaterialFromMap(UPhysicalMaterialMask::GetPhysMatIndex(MaskData, MaskSizeX, MaskSizeY, MaskAdressX, MaskAdressY, TextureUV_T0.X, TextureUV_T0.Y));
						}
						if (!PhysicalMaterial)
						{
							PhysicalMaterial = LandscapeProxy->GetLandscapeMaterial() ? LandscapeProxy->GetLandscapeMaterial()->GetPhysicalMaterial() : nullptr;
						}
						if (!PhysicalMaterial)
						{
							PhysicalMaterial = LandscapeProxy->DefaultPhysMaterial;
						}
						int32* IndexMaterial = PhysicalMaterial2ID.Find(Cast<UStalkerPhysicalMaterial>(PhysicalMaterial));
						Triangle.MaterialIndex = IndexMaterial ? static_cast<uint32>(*IndexMaterial) : DefaultID;

						Triangle.VertexIndex0 = StartIndex;
						Triangle.VertexIndex2 = StartIndex + 2;
						Triangle.VertexIndex1 = StartIndex + 3;
						CForm->Triangles.Add(Triangle);

						FVector2D TextureUV_T1 = (((TextureUV0 + TextureUV1 + TextureUV2) / 3.f) - FVector2D(MinX, MinY)) * UVScale;

						PhysicalMaterial = nullptr;
						if (UseMask)
						{
							PhysicalMaterial = LandscapeProxy->GetLandscapeMaterial()->GetPhysicalMaterialFromMap(UPhysicalMaterialMask::GetPhysMatIndex(MaskData, MaskSizeX, MaskSizeY, MaskAdressX, MaskAdressY, TextureUV_T1.X, TextureUV_T1.Y));
						}
						if (!PhysicalMaterial)
						{
							PhysicalMaterial = LandscapeProxy->GetLandscapeMaterial() ? LandscapeProxy->GetLandscapeMaterial()->GetPhysicalMaterial() : nullptr;
						}
						if (!PhysicalMaterial)
						{
							PhysicalMaterial = LandscapeProxy->DefaultPhysMaterial;
						}
						IndexMaterial = PhysicalMaterial2ID.Find(Cast<UStalkerPhysicalMaterial>(PhysicalMaterial));
						Triangle.MaterialIndex = IndexMaterial ? static_cast<uint32>(*IndexMaterial) : DefaultID;
						Triangle.VertexIndex0 = StartIndex;
						Triangle.VertexIndex2 = StartIndex + 1;
						Triangle.VertexIndex1 = StartIndex + 2;
						CForm->Triangles.Add(Triangle);

					}
				}
				CForm->AABB += FBox3f(LandscapeComponent->Bounds.GetBox());
			}
		}
		
	}

	PhysicalMaterial2ID.Empty(PhysicalMaterial2ID.Num());


	GStalkerEngineManager->GetPhysicalMaterialsManager()->Clear();
	CForm->Modify();
	if (CForm->Triangles.Num() == 0)
	{
		UE_LOG(LogStalkerEditor,Warning,TEXT("CFrom is empty in world %s"),*World->GetPathName())
	}
}
