#include "RBMKDetailModel.h"

#include "PhysicsEngine/BodySetup.h"
#include "StalkerEditor/StalkerEditorManager.h"
#include "StalkerEditor/Importer/FRBMKEngineFactory.h"

FRBMKDetail::FRBMKDetail()
{
	ReferencePtr		= nullptr;
    MinScale			= 0.5f;
    MaxScale   		    = 2.f;
    DensityFactor	    = 1.f;
}

FRBMKDetail::~FRBMKDetail()
{
    if(ReferencePtr)
    {
		GXRayObjectLibrary->RemoveEditObject(ReferencePtr);
    }
}

bool FRBMKDetail::Update	(const FString&Name)
{
    
	auto IsDegeneratedLambda = [](int32 InIndices[3])
	{
		return (InIndices[0]==InIndices[1] || InIndices[0]==InIndices[2] || InIndices[1]==InIndices[2]);
	};
    TArray<int32> IsEqualTemp;
    auto IsEqualLambda = [&IsEqualTemp](int32 InIndices[3])
	{
    	for (int32 i = 0;IsEqualTemp.Num(); i+=3)
	    {
            if(IsEqualTemp[i + 0] == InIndices[0]&&IsEqualTemp[i + 1] == InIndices[1]&&IsEqualTemp[i + 2] == InIndices[2])return true;
            if(IsEqualTemp[i + 0] == InIndices[0]&&IsEqualTemp[i + 2] == InIndices[1]&&IsEqualTemp[i + 1] == InIndices[2])return true;
            if(IsEqualTemp[i + 2] == InIndices[0]&&IsEqualTemp[i + 0] == InIndices[1]&&IsEqualTemp[i + 1] == InIndices[2])return true;
            if(IsEqualTemp[i + 2] == InIndices[0]&&IsEqualTemp[i + 1] == InIndices[1]&&IsEqualTemp[i + 0] == InIndices[2])return true;
            if(IsEqualTemp[i + 1] == InIndices[0]&&IsEqualTemp[i + 0] == InIndices[1]&&IsEqualTemp[i + 2] == InIndices[2])return true;
            if(IsEqualTemp[i + 1] == InIndices[0]&&IsEqualTemp[i + 2] == InIndices[1]&&IsEqualTemp[i + 0] == InIndices[2])return true;
	    }
	    return false;
	};
	ReferenceName = Name;


    CEditableObject* InReference	= GXRayObjectLibrary->CreateEditObject(TCHAR_TO_ANSI(*Name));
    if (!InReference)
    {
		UE_LOG(LogXRayImporter,Error,TEXT("Can't load detail object '%s'."),*Name);
        return false;
    }
    if(InReference->SurfaceCount() != 1)
    {
        UE_LOG(LogXRayImporter,Error,TEXT("Detail object '%s' must contain 1 material."),*Name);
	    GXRayObjectLibrary->RemoveEditObject(InReference);
    	return false;
    }
	if(InReference->MeshCount() == 0)
    {
        UE_LOG(LogXRayImporter,Error,TEXT("Detail object '%s' must contain 1 mesh."),*Name);
	    GXRayObjectLibrary->RemoveEditObject(InReference);
    	return false;
    }

    GXRayObjectLibrary->RemoveEditObject(ReferencePtr);
    ReferencePtr				= InReference;

    CEditableMesh* Mesh	= *ReferencePtr->FirstMesh();
    FBox3f Box;
    for (u32 FaceID=0; FaceID<Mesh->GetFCount(); FaceID++)
    {
		const  st_Face& Face = Mesh->GetFaces()[FaceID];
    	int32 Triangle[3];
    	for (int32 VertexID=0; VertexID<3; VertexID++)
        {
            FRBMKDetailVertex Vertex;

    		Vertex.Position = StalkerMath::XRayLocationToUnreal( Mesh->GetVertices()[Face.pv[VertexID].pindex]);

            const st_VMapPt&VertexMap = Mesh->GetVMRefs()[Face.pv[VertexID].vmref].pts[0];
            const Fvector2 InUV = Mesh->GetVMaps()[VertexMap.vmap_index]->getUV(VertexMap.index);
			Vertex.UV.X =  InUV.x;
			Vertex.UV.Y =  InUV.y;

        	Triangle[VertexID] = Vertices.Find(Vertex);
            if(Triangle[VertexID] == INDEX_NONE)
            {
	             Box += Vertex.Position;
                 Triangle[VertexID] = Vertices.Add(Vertex);
            }
        }
        if (IsDegeneratedLambda(Triangle))	continue;
        if (IsEqualLambda(Triangle))	continue;
		Indices.Add(Triangle[0]);
		Indices.Add(Triangle[1]);
        Indices.Add(Triangle[2]);
    }
    Bound = FBoxSphereBounds3f(Box);

	{
        CSurface* Surface		= *ReferencePtr->FirstSurface();
		if(ensure(Surface))
		{
			ShaderName = Surface->_ShaderName();
			TextureName	= Surface->_Texture();
		}
	}
    return true;
}

UStaticMesh* FRBMKDetail::GetOrCreateStaticMesh(EObjectFlags InFlags)
{
    FRBMKEngineFactory EngineFactory(nullptr,InFlags);

	FString NewObjectPath = GStalkerEditorManager->GetGamePath() / TEXT("Maps") / TEXT("Meshes") / GetName();
    NewObjectPath.ReplaceCharInline(TEXT('\\'), TEXT('/'));

	UStaticMesh* StaticMesh = nullptr;
	if(!FRBMKEngineFactory::LoadOrCreateOrOverwriteAsset(NewObjectPath,InFlags,StaticMesh))
	{
		return StaticMesh;
	}

	UE_LOG(LogXRayImporter, Log, TEXT("Import detail object %s"), *GetName());
	TArray<FStaticMaterial> Materials;
    FAssetRegistryModule::AssetCreated(StaticMesh);
    Materials.AddUnique(FStaticMaterial(EngineFactory.ImportSurface(NewObjectPath + TEXT("_Mat"), TCHAR_TO_ANSI(*ShaderName), TCHAR_TO_ANSI(*TextureName), "", false), TEXT("root_mat"), TEXT("root_mat")));
    {
		const int32 LodIndex = 0;
    	FStaticMeshSourceModel& SourceModel = StaticMesh->AddSourceModel();
		FMeshDescription* MeshDescription = StaticMesh->CreateMeshDescription(LodIndex);
		if (MeshDescription)
		{
			FStaticMeshAttributes StaticMeshAttributes(*MeshDescription);
			TVertexAttributesRef<FVector3f> VertexPositions = StaticMeshAttributes.GetVertexPositions();
			TEdgeAttributesRef<bool>  EdgeHardnesses = StaticMeshAttributes.GetEdgeHardnesses();
			TPolygonGroupAttributesRef<FName> PolygonGroupImportedMaterialSlotNames = StaticMeshAttributes.GetPolygonGroupMaterialSlotNames();
			TVertexInstanceAttributesRef<FVector2f> VertexInstanceUVs = StaticMeshAttributes.GetVertexInstanceUVs();
			{
				VertexInstanceUVs.SetNumChannels(1);
				FPolygonGroupID CurrentPolygonGroupID = MeshDescription->CreatePolygonGroup();
				PolygonGroupImportedMaterialSlotNames[CurrentPolygonGroupID] = TEXT("root_mat");
				TArray<FVertexInstanceID> VertexInstanceIDs;
				VertexInstanceIDs.SetNum(3);
				for (size_t FaceID = 0; FaceID < Indices.Num() / 3; FaceID++)
				{
					for (size_t VirtualVertexID = 0; VirtualVertexID < 3; VirtualVertexID++)
					{
						static size_t VirtualVertices[3] = { 0,2,1 };
						size_t VertexID = Indices[VirtualVertices[VirtualVertexID] + FaceID * 3];
						FVertexID VertexIDs = MeshDescription->CreateVertex();
						VertexPositions[VertexIDs] = Vertices[VertexID].Position;
						VertexInstanceIDs[VirtualVertexID] = MeshDescription->CreateVertexInstance(VertexIDs);
						VertexInstanceUVs.Set(VertexInstanceIDs[VirtualVertexID], 0, Vertices[VertexID].UV);
					}
					const FPolygonID NewPolygonID = MeshDescription->CreatePolygon(CurrentPolygonGroupID, VertexInstanceIDs);
				}
			}
		}

		StaticMesh->CommitMeshDescription(LodIndex);
		SourceModel.BuildSettings.bRecomputeNormals = true;
		SourceModel.BuildSettings.bGenerateLightmapUVs = true;
		SourceModel.BuildSettings.DstLightmapIndex = 1;
		SourceModel.BuildSettings.MinLightmapResolution = 128;
    }
    StaticMesh->SetStaticMaterials(Materials);
	StaticMesh->Build();
    {
	    FMeshSectionInfo MeshSectionInfo = StaticMesh->GetSectionInfoMap().Get(0, 0);
		MeshSectionInfo.bCastShadow = true;
		MeshSectionInfo.bEnableCollision = false;
		MeshSectionInfo.bAffectDistanceFieldLighting = false;
		MeshSectionInfo.bVisibleInRayTracing = false;
		MeshSectionInfo.MaterialIndex = 0;
		StaticMesh->GetSectionInfoMap().Set(0, 0, MeshSectionInfo);
    }
	StaticMesh->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseDefault;
	StaticMesh->Modify();
	StaticMesh->PostEditChange();
    return StaticMesh;
}

UFoliageType_InstancedStaticMesh* FRBMKDetail::GetOrCreateFoliageType(EObjectFlags InFlags)
{
	FString NewObjectPath = GStalkerEditorManager->GetGamePath() / TEXT("Maps") / TEXT("Meshes") / GetName() + TEXT("_FoliageType");
    NewObjectPath.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	UFoliageType_InstancedStaticMesh *FoliageType = nullptr;

	if(!FRBMKEngineFactory::LoadOrCreateOrOverwriteAsset(NewObjectPath,InFlags,FoliageType))
	{
		return FoliageType;
	}

    FAssetRegistryModule::AssetCreated(FoliageType);
	FoliageType->SetStaticMesh(GetOrCreateStaticMesh(InFlags));
	FoliageType->ScaleX.Max = MaxScale;
	FoliageType->ScaleX.Max = MinScale;
	FoliageType->CullDistance.Max = 7500;
	FoliageType->CullDistance.Min = 7400;
	FoliageType->bEnableCullDistanceScaling = true;
	FoliageType->Modify();
	FoliageType->PostEditChange();
	return FoliageType;
}


bool FRBMKDetail::Load(IReader& F)
{
	enum
	{
		DETOBJ_CHUNK_VERSION = 0x1000,
		DETOBJ_CHUNK_REFERENCE = 0x0101,                           
		DETOBJ_CHUNK_SCALE_LIMITS = 0x0102,
		DETOBJ_CHUNK_DENSITY_FACTOR = 0x0103,
		DETOBJ_CHUNK_FLAGS = 0x0104,
		DETOBJ_VERSION = 0x0001,
	};
    string256 InName;


    check(F.find_chunk(DETOBJ_CHUNK_VERSION));

    if (!ensure(F.r_u32() == DETOBJ_VERSION))
    {
        return false;
    }

    check(F.find_chunk(DETOBJ_CHUNK_REFERENCE));
    F.r_stringZ(InName,sizeof(InName));

    check(F.find_chunk(DETOBJ_CHUNK_SCALE_LIMITS));

    MinScale    = F.r_float();
	if (FMath::IsNearlyZero(MinScale))	
	{
		MinScale = 0.1f;
	}

	MaxScale = F.r_float();
	if (MaxScale<MinScale)
	{
		MaxScale = MinScale;
	}

    if (F.find_chunk(DETOBJ_CHUNK_DENSITY_FACTOR))
    {
    	DensityFactor = F.r_float();
    }

    if (F.find_chunk(DETOBJ_CHUNK_FLAGS))
    {
    	Flags.assign(F.r_u32());
    }

    return  Update(ANSI_TO_TCHAR(InName));
}

FString FRBMKDetail::GetName() const
{
	if(ReferencePtr)
	{
		return ReferencePtr->GetName();
	}
    return ReferenceName;
}
