#include "XRayEngineFactory.h"


XRayEngineFactory::XRayEngineFactory(UObject* InParentPackage, EObjectFlags InFlags)
{
	ParentPackage = InParentPackage;
	ObjectFlags = InFlags;
}

XRayEngineFactory::~XRayEngineFactory()
{	
	
}

UStaticMesh* XRayEngineFactory::ImportObjectAsStaticMesh(const FString& FileName)
{
	CEditableObject Object("Temp");


	if (Object.Load(TCHAR_TO_ANSI(*FileName)))
	{
		const FString PackageName = UPackageTools::SanitizePackageName(ParentPackage->GetName() / FPaths::GetBaseFilename(FileName));
		UPackage* AssetPackage = CreatePackage(*PackageName);
		UStaticMesh*  StaticMesh = NewObject<UStaticMesh>(AssetPackage, *FPaths::GetBaseFilename(PackageName), ObjectFlags);
		FAssetRegistryModule::AssetCreated(StaticMesh);
		TArray<FStaticMaterial> Materials;
		FStaticMeshSourceModel& SourceModel = StaticMesh->AddSourceModel();
		int32 MaterialIndex = 0;
		for (int32 LodIndex = 0; LodIndex < 1; LodIndex++)
		{
			FMeshDescription* MeshDescription = StaticMesh->CreateMeshDescription(LodIndex);
			if (MeshDescription)
			{
				FStaticMeshAttributes StaticMeshAttributes(*MeshDescription);
				TVertexAttributesRef<FVector3f> VertexPositions = StaticMeshAttributes.GetVertexPositions();
				TEdgeAttributesRef<bool>  EdgeHardnesses = StaticMeshAttributes.GetEdgeHardnesses();
				TPolygonGroupAttributesRef<FName> PolygonGroupImportedMaterialSlotNames = StaticMeshAttributes.GetPolygonGroupMaterialSlotNames();
				TVertexInstanceAttributesRef<FVector3f> VertexInstanceNormals = StaticMeshAttributes.GetVertexInstanceNormals();
				TVertexInstanceAttributesRef<FVector3f> VertexInstanceTangents = StaticMeshAttributes.GetVertexInstanceTangents();
				TVertexInstanceAttributesRef<float> VertexInstanceBinormalSigns = StaticMeshAttributes.GetVertexInstanceBinormalSigns();
				TVertexInstanceAttributesRef<FVector2f> VertexInstanceUVs = StaticMeshAttributes.GetVertexInstanceUVs();
				TVertexInstanceAttributesRef<FVector4f> VertexInstanceColors = StaticMeshAttributes.GetVertexInstanceColors();

				for (size_t ElementID = 0; ElementID < Object.SurfaceCount(); ElementID++)
				{
					xr_vector< st_StaticMeshVertex> Vertices;
					for (size_t MeshID = 0; MeshID < Object.MeshCount(); MeshID++)
					{
						Object.Meshes()[MeshID]->GenerateVertices(Vertices, Object.Surfaces()[ElementID]);
					}
					if (Vertices.size() == 0)
					{
						continue;
					}
					VertexInstanceUVs.SetNumChannels(1);
					FPolygonGroupID CurrentPolygonGroupID = MeshDescription->CreatePolygonGroup();
					PolygonGroupImportedMaterialSlotNames[CurrentPolygonGroupID] = *FString::Printf(TEXT("mat_%d"), MaterialIndex);
					TArray<FVertexInstanceID> VertexInstanceIDs;
					VertexInstanceIDs.SetNum(3);
					for (size_t FaceID = 0; FaceID < Vertices.size()/3; FaceID++)
					{
						for (size_t VirtualVertexID = 0; VirtualVertexID <3; VirtualVertexID++)
						{
							static size_t VirtualVertices[3] ={0,1,2};
							size_t VertexID = VirtualVertices[ VirtualVertexID]+ FaceID*3;
							FVertexID VertexIDs = MeshDescription->CreateVertex();
							VertexPositions[VertexIDs].X = -Vertices[VertexID].Position.x * 100;
							VertexPositions[VertexIDs].Y = Vertices[VertexID].Position.z * 100;
							VertexPositions[VertexIDs].Z = Vertices[VertexID].Position.y * 100;
							VertexInstanceIDs[VirtualVertexID] = MeshDescription->CreateVertexInstance(VertexIDs);
							FVector2f UV;
							UV.X = Vertices[VertexID].UV.x;
							UV.Y = Vertices[VertexID].UV.y;
							VertexInstanceUVs.Set(VertexInstanceIDs[VirtualVertexID], 0, UV);
							VertexInstanceNormals[VertexInstanceIDs[VirtualVertexID]].X = -Vertices[VertexID].Normal.x;
							VertexInstanceNormals[VertexInstanceIDs[VirtualVertexID]].Y = Vertices[VertexID].Normal.z;
							VertexInstanceNormals[VertexInstanceIDs[VirtualVertexID]].Z = Vertices[VertexID].Normal.y;
						}
						const FPolygonID NewPolygonID = MeshDescription->CreatePolygon(CurrentPolygonGroupID, VertexInstanceIDs);
					}
				//	Materials.AddUnique(FStaticMaterial(CreateMaterial(ImporterStaticMesh->GetIndexMaterial(ElementID, LodIndex), ParentPackage, Flags, Options), *FString::Printf(TEXT("mat_%d"), MaterialIndex), *FString::Printf(TEXT("mat_%d"), MaterialIndex)));
				}
			}

			StaticMesh->CommitMeshDescription(LodIndex);
		}
		SourceModel.BuildSettings.bRecomputeNormals = false;
		SourceModel.BuildSettings.bGenerateLightmapUVs = true;
		SourceModel.BuildSettings.DstLightmapIndex = 1;
		SourceModel.BuildSettings.MinLightmapResolution = 128;
		StaticMesh->SetStaticMaterials(Materials);
		StaticMesh->Build();
		StaticMesh->MarkPackageDirty();
		StaticMesh->PostEditChange();
		return StaticMesh;
	}
	return nullptr;
}
