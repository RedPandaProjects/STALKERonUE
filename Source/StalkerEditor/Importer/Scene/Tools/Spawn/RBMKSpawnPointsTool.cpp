#include "RBMKSpawnPointsTool.h"

#include "Entities/Scene/SpawnObject/Components/StalkerSpawnObjectBoxShapeComponent.h"
#include "Entities/Scene/SpawnObject/Components/StalkerSpawnObjectSphereShapeComponent.h"
#include "Entities/Scene/SpawnObject/StalkerSpawnObject.h"
#include "Importer/Scene/Entitys/Shape/RBMKSceneShape.h"
#include "Importer/Scene/Entitys/SpawnPoint//RBMKSceneSpawnPoint.h"
#include "Importer/Scene/Entitys/StaticMesh//RBMKSceneStaticMesh.h"
#include "Importer/Scene/GeometryPartExtractor.h"
#include "Importer/Scene/Tools/StaticMesh/RBMKSceneStaticMeshesTool.h"

THIRD_PARTY_INCLUDES_START
#include "Editors/XrECore/Engine/GameMtlLib.h"
THIRD_PARTY_INCLUDES_END

FRBMKSpawnPointsTool::FRBMKSpawnPointsTool():FRBMKSceneObjectsToolBase(ERBMKSceneObjectType::SpawnPoint)
{
}

FRBMKSpawnPointsTool::~FRBMKSpawnPointsTool()
{
}

TSharedPtr<FRBMKSceneObjectBase> FRBMKSpawnPointsTool::CreateObject(const FString&Name)
{
	return MakeShared<FRBMKSceneSpawnPoint>(Name);
}

void FRBMKSpawnPointsTool::ExportToWorld(UWorld* World, EObjectFlags InFlags,const URBMKLevelImportOptions&LevelImportOptions)
{
	for (TSharedPtr<FRBMKSceneObjectBase>& Object : Objects)
	{
		FRBMKSceneSpawnPoint* SpawnObject = reinterpret_cast<FRBMKSceneSpawnPoint*>(Object->QueryInterface(ERBMKSceneObjectType::SpawnPoint));
		if (SpawnObject && SpawnObject->SpawnData.GetEntity())
		{
			AStalkerSpawnObject* SpawnObjectActor = World->SpawnActor<AStalkerSpawnObject>(AStalkerSpawnObject::StaticClass(), SpawnObject->GetTransform());
			SpawnObjectActor->SetFolderPath(TEXT("Spawns"));
			SpawnObjectActor->DestroyEntity();
			SpawnObjectActor->SetActorLabel(SpawnObject->GetName());
			{
				SpawnObjectActor->SectionName = ANSI_TO_TCHAR(SpawnObject->SpawnData.GetEntity()->name());
				SpawnObjectActor->EntityData.Reset();
				NET_Packet 			Packet;
				SpawnObject->SpawnData.GetEntity()->Spawn_Write(Packet, TRUE);
				for (u32 i = 0; i < Packet.B.count; i++)
				{
					SpawnObjectActor->EntityData.Add(Packet.B.data[i]);
				}
			}
			SpawnObjectActor->SpawnRead();

			if (SpawnObject->AttachedObject)
			{
				if (FRBMKSceneShape* Shape = reinterpret_cast<FRBMKSceneShape*>(SpawnObject->AttachedObject->QueryInterface(ERBMKSceneObjectType::Shape)))
				{
					for (CShapeData::shape_def& ShapeData : Shape->GetShapes())
					{
						if (ShapeData.type == CShapeData::cfSphere)
						{
							UStalkerSpawnObjectSphereShapeComponent* NewShapeComponent = NewObject< UStalkerSpawnObjectSphereShapeComponent>(SpawnObjectActor, NAME_None, RF_Transactional);
							SpawnObjectActor->AddInstanceComponent(NewShapeComponent);
							NewShapeComponent->AttachToComponent(SpawnObjectActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
							NewShapeComponent->OnComponentCreated();
							NewShapeComponent->RegisterComponent();
							FTransform ShapeTransform;
							ShapeTransform.SetLocation(FVector(StalkerMath::RBMKLocationToUnreal(ShapeData.data.sphere.P)) + Shape->GetTransform().GetLocation());
							NewShapeComponent->SphereRadius = ShapeData.data.sphere.R * 100.f;
							NewShapeComponent->SetWorldTransform(ShapeTransform);
							NewShapeComponent->UpdateBounds();
							NewShapeComponent->MarkRenderStateDirty();
						}
						else if (ShapeData.type == CShapeData::cfBox)
						{
							UStalkerSpawnObjectBoxShapeComponent* NewShapeComponent = NewObject< UStalkerSpawnObjectBoxShapeComponent>(SpawnObjectActor, NAME_None, RF_Transactional);
							SpawnObjectActor->AddInstanceComponent(NewShapeComponent);
							NewShapeComponent->AttachToComponent(SpawnObjectActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
							NewShapeComponent->OnComponentCreated();
							NewShapeComponent->RegisterComponent();

							Fmatrix MBox = ShapeData.data.box;
							MBox.mulA_43(StalkerMath::UnrealMatrixToRBMK(FMatrix(Shape->GetTransform().ToMatrixWithScale())));

							FTransform ShapeTransform;
							ShapeTransform.SetLocation(FVector(StalkerMath::RBMKLocationToUnreal(MBox.c)));
							{
								Fvector A, B[8];
								A.set(-.5f, -.5f, -.5f);	MBox.transform_tiny(B[0], A);
								A.set(-.5f, -.5f, +.5f);	MBox.transform_tiny(B[1], A);
								A.set(-.5f, +.5f, +.5f);	MBox.transform_tiny(B[2], A);
								A.set(-.5f, +.5f, -.5f);	MBox.transform_tiny(B[3], A);
								A.set(+.5f, +.5f, +.5f);	MBox.transform_tiny(B[4], A);
								A.set(+.5f, +.5f, -.5f);	MBox.transform_tiny(B[5], A);
								A.set(+.5f, -.5f, +.5f);	MBox.transform_tiny(B[6], A);
								A.set(+.5f, -.5f, -.5f);	MBox.transform_tiny(B[7], A);

								Fvector TangetX;
								Fvector TangetY;
								Fvector TangetZ;
								TangetX.sub(B[0], B[7]);
								TangetX.normalize_safe();
								TangetY.sub(B[0], B[3]);
								TangetY.normalize_safe();
								TangetZ.sub(B[0], B[1]);
								TangetZ.normalize_safe();
								FVector UnrealTangetX = FVector(StalkerMath::RBMKNormalToUnreal(TangetX));
								FVector UnrealTangetZ = FVector(StalkerMath::RBMKNormalToUnreal(TangetY));
								FVector UnrealTangetY = FVector(StalkerMath::RBMKNormalToUnreal(TangetZ));

								FMatrix MatrixRotate(UnrealTangetX, UnrealTangetY, UnrealTangetZ, FVector(0, 0, 0));
								ShapeTransform.SetRotation(MatrixRotate.ToQuat());
								ShapeTransform.SetScale3D(FVector(B[0].distance_to(B[7]), B[0].distance_to(B[1]), B[0].distance_to(B[3])) / 2.f);
							}
							NewShapeComponent->SetWorldTransform(ShapeTransform);
						}
					}

				}

			}
		}
	}
	{
		CGameMtlLibrary Library;
		XrGameMaterialLibraryInterface* LastGameMaterialLibrary = GameMaterialLibrary;
		Library.Load();
		GameMaterialLibrary = &Library;

		auto BuildMesh = [] (const Fmatrix& Parent, CEditableMesh* Mesh, CGeomPartExtractor* Extractor, u32 GameMaterialMask)
		{
			bool bResult 			= true;
			Mesh->GenerateVNormals	(&Parent);
			for (auto &[Surface,FaceList] : Mesh->GetSurfFaces())
			{
				const int32 GameMaterialID = Surface->_GameMtl();

				if (GameMaterialID==GAMEMTL_NONE_ID)
				{
					UE_LOG(LogXRayImporter,Error,TEXT("Object '%S', surface '%S' contain invalid game material."),Mesh->Parent()->m_LibName.c_str(),Surface->_Name());
        			bResult 		= FALSE; 
					break; 
				}

				const SGameMtl* GameMtl = GameMaterialLibrary->GetMaterialByID(GameMaterialID);

				if (GameMtl == nullptr)
				{
					UE_LOG(LogXRayImporter,Error,TEXT("Object '%S', surface '%S' contain undefined game material."),Mesh->Parent()->m_LibName.c_str(),Surface->_Name());
        			bResult 		= FALSE; 
					break; 
				}

				if (!GameMtl->Flags.is(GameMaterialMask))
				{
					continue;
				}

				const st_Face* Faces = Mesh->GetFaces(); check(Faces);
				const Fvector* VNormals = Mesh->GetVNormals(); check(VNormals);
				const Fvector* Vertices = Mesh->GetVertices(); check(Vertices);

				for (IntVec::const_iterator f_it=FaceList.begin(); f_it!=FaceList.end(); ++f_it)
				{
					const st_Face& Face = Faces[*f_it];
					Fvector WorldVertices[3],WorldNormals[3];
					Parent.transform_tiny	(WorldVertices[0],Vertices[Face.pv[0].pindex]);
					Parent.transform_tiny	(WorldVertices[1],Vertices[Face.pv[1].pindex]);
					Parent.transform_tiny	(WorldVertices[2],Vertices[Face.pv[2].pindex]);
					Parent.transform_dir	(WorldNormals[0],VNormals[*f_it*3+0]); WorldNormals[0].normalize();
					Parent.transform_dir	(WorldNormals[1],VNormals[*f_it*3+1]); WorldNormals[1].normalize();
					Parent.transform_dir	(WorldNormals[2],VNormals[*f_it*3+2]); WorldNormals[2].normalize();
					const Fvector2*	UV[3];
					Mesh->GetFaceTC(*f_it,UV);
					Extractor->AppendFace(WorldVertices,WorldNormals,UV);
				}
				if (!bResult) break;
			}
			Mesh->UnloadVNormals();
			return bResult;
		};

		auto OrientToNorm = [] (Fvector& LocalNormal, Fmatrix33& Form, Fvector& HalfSize)
		{
			Fvector* FormPointer = reinterpret_cast<Fvector*>(&Form);
			int32 MaxProj = 0, MinSize = 0;
			for (int32 k = 1; k < 3; k++) 
			{
				if (FMath::Abs(LocalNormal[k]) > FMath::Abs(LocalNormal[MaxProj]))
					MaxProj = k;
				if (HalfSize[k] < HalfSize[MinSize])
					MinSize = k;
			}
			if (MinSize != MaxProj) return false;
			if (LocalNormal[MaxProj] < 0.f)
			{
				LocalNormal.invert();
				FormPointer[MaxProj].invert();
				FormPointer[(MaxProj + 1) % 3].invert();
			}
			return true;
		};

		CGeomPartExtractor* GeomPartExtractor = nullptr;
		FRBMKSceneStaticMeshesTool* StaticMeshesTool = static_cast<FRBMKSceneStaticMeshesTool*>(GRBMKScene->GetTool(ERBMKSceneObjectType::StaticMesh));
		Fbox BoundBox;
		GeomPartExtractor  = new CGeomPartExtractor;
		GeomPartExtractor->Initialize(BoundBox,EPS_L,int_max);

		{
			for (TSharedPtr<FRBMKSceneObjectBase>& Object : StaticMeshesTool->Objects)
			{
				FRBMKSceneStaticMesh* StaticMesh = reinterpret_cast<FRBMKSceneStaticMesh*>(Object->QueryInterface(ERBMKSceneObjectType::StaticMesh));
				if (StaticMesh->IsStatic())
				{
					CEditableObject *MeshObject = StaticMesh->GetReferenceObject();
					const Fmatrix XForm = StalkerMath::UnrealMatrixToRBMK(StaticMesh->GetTransform().ToMatrixWithScale());
            
					for(EditMeshIt MeshPart =MeshObject->FirstMesh(); MeshPart!=MeshObject->LastMesh(); ++MeshPart)
					{
						if (!BuildMesh	(XForm, *MeshPart, GeomPartExtractor, SGameMtl::flClimable))
						{
							break;
						}
					}
				
				}
			}
		}
		GeomPartExtractor->Process();
		{
			int32 Index = 0;
		    for (SBPart* Part: GeomPartExtractor->GetParts())
			{
        		if (Part->Valid())
				{
					Fvector LocalNormal = {0,0,0};

					for (SBFace* Face : Part->m_Faces)
					{
                		for (int32 k=0; k<3; k++)
                		{
                			LocalNormal.add(Face->n[k]);
                		}
					}

					LocalNormal.normalize_safe		();

	          		if (!OrientToNorm(LocalNormal,Part->m_OBB.m_rotate,Part->m_OBB.m_halfsize))
					{
                    	continue;
					}
					else
					{
						Fmatrix WorldMatrix;
						WorldMatrix.set(Part->m_OBB.m_rotate.i,Part->m_OBB.m_rotate.j,Part->m_OBB.m_rotate.k,Part->m_OBB.m_translate);
						FMatrix InM = StalkerMath::RBMKMatrixToUnreal(WorldMatrix);
						AStalkerSpawnObject* SpawnObjectActor = World->SpawnActor<AStalkerSpawnObject>();
						SpawnObjectActor->SetActorTransform(FTransform(InM));
						SpawnObjectActor->SetFolderPath(TEXT("Spawns"));
						SpawnObjectActor->DestroyEntity();
						SpawnObjectActor->SetActorLabel(FString::Printf(TEXT("clmbl#%d"),Index++));
						SpawnObjectActor->SectionName = "climable_object";
						SpawnObjectActor->SpawnRead();
						{
							UStalkerSpawnObjectBoxShapeComponent* NewShapeComponent = NewObject< UStalkerSpawnObjectBoxShapeComponent>(SpawnObjectActor, NAME_None, RF_Transactional);
							SpawnObjectActor->AddInstanceComponent(NewShapeComponent);
							NewShapeComponent->AttachToComponent(SpawnObjectActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
							NewShapeComponent->OnComponentCreated();
							NewShapeComponent->RegisterComponent();
							NewShapeComponent->BoxExtent.Set((Part->m_BBox.max.x-Part->m_BBox.min.x)*50,(Part->m_BBox.max.z-Part->m_BBox.min.z)*50,(Part->m_BBox.max.y-Part->m_BBox.min.y)*50);
						}
					}
				}
				else
				{
					UE_LOG(LogXRayImporter,Error,TEXT("Can't export invalid part #%d"),Index++);
				}
			}
		}

		delete GeomPartExtractor;
		GameMaterialLibrary = LastGameMaterialLibrary;
	}
}


bool FRBMKSpawnPointsTool::LoadLTX(CInifile& INIFile)
{
	const uint32 Version 	= INIFile.r_u32("main","version");
    if(Version != ToolVersion )
    {
		UE_LOG(LogXRayImporter,Error,TEXT("%s tools: Unsupported version."),ClassName());
        return false;
    }
    return FRBMKSceneObjectsToolBase::LoadLTX	(INIFile);
}

bool FRBMKSpawnPointsTool::LoadStream(IReader& F)
{
	uint16 Version 	= 0;
    if(F.r_chunk(ChunkVersion,&Version))
    {
	    if(Version != ToolVersion)
        {
	        UE_LOG(LogXRayImporter,Error,TEXT("%s tools: Unsupported version."),ClassName());
	        return false;
	    }
    }
    return FRBMKSceneObjectsToolBase::LoadStream	(F);
}


bool FRBMKSpawnPointsTool::LoadSelection(IReader& F)
{
	uint16 Version 	= 0;
    if(F.r_chunk(ChunkVersion,&Version))
    {
	    if(Version != ToolVersion)
        {
	        UE_LOG(LogXRayImporter,Error,TEXT("%s tools: Unsupported version."),ClassName());
	        return false;
	    }
    }
    return FRBMKSceneObjectsToolBase::LoadSelection	(F);
}

