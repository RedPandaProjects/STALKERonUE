#include "RBMKLevelFactory.h"

#include "NiagaraActor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Scene/Entitys/StaticObject/SceneObject.h"
#include "Scene/Tools/AIMap/ESceneAIMapTools.h"
#include "Kernel/Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/AIMap/StalkerAIMap.h"
#include "Scene/Entitys/WayObject/WayPoint.h"
#include "../Entities/Scene/WayObject/StalkerWayObject.h"
#include "../Entities/Scene/WayObject/StalkerWayPointComponent.h"
#include "Scene/Entitys/SpawnObject/SpawnPoint.h"
#include "../Entities/Scene/SpawnObject/StalkerSpawnObject.h"
#include "../Entities/Scene/SpawnObject/Components/StalkerSpawnObjectBoxShapeComponent.h"
#include "Scene/Entitys/ShapeObject/EShape.h"
#include "../Entities/Scene/SpawnObject/Components/StalkerSpawnObjectSphereShapeComponent.h"
#include "ImporterFactory/XRayLevelImportOptions.h"
#include "Scene/GeometryPartExtractor.h"
#include "Scene/Tools/StaticObject/ESceneObjectTools.h"
#include "Resources/StaticMesh/StalkerStaticMeshAssetUserData.h"
#include "Scene/Entitys/ParticlesObject/ParticlesObject.h"
#include "Scene/Tools/Detail/RBMKSceneDetailObjectTool.h"
#include "Scene/Tools/Wallmark/ESceneWallmarkTool.h"
#include "StalkerEditor/StalkerEditorManager.h"
THIRD_PARTY_INCLUDES_START
#include "Editors/XrECore/Engine/GameMtlLib.h"
THIRD_PARTY_INCLUDES_END


RBMKLevelFactory::RBMKLevelFactory(UObject* InParentPackage, EObjectFlags InFlags):EngineFactory(InParentPackage, InFlags),ParentPackage(InParentPackage),ObjectFlags(InFlags)
{

}

RBMKLevelFactory::~RBMKLevelFactory()
{

}

bool RBMKLevelFactory::ImportLevel(const FString& FileName,UXRayLevelImportOptions&LevelImportOptions)
{

	GXRayObjectLibrary->AngleSmooth = LevelImportOptions.AngleNormalSmoth;
	switch (LevelImportOptions.ObjectImportGameFormat)
	{
	default:
		GXRayObjectLibrary->LoadAsGame = EGame::NONE;
		break;
	case EXRayObjectImportGameFormat::SOC:
		GXRayObjectLibrary->LoadAsGame = EGame::SHOC;
		break;
	case EXRayObjectImportGameFormat::CS_COP:
		GXRayObjectLibrary->LoadAsGame = EGame::COP;
		break;
	}
	GXRayObjectLibrary->ReloadObjects();
	FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	if (!WorldContext)
		return false;
	UWorld* World = WorldContext->World();
	if (!IsValid(World))
		return false;

	EScene CurrentScene; 
	Scene = &CurrentScene;
	bool IsLtx = false;
	{
		IReader* R = FS.r_open(TCHAR_TO_ANSI(*FileName));
		if (!R)
		{
			return false;
		}
		char ch;
		R->r(&ch, sizeof(ch));
		IsLtx = (ch == '[');
		FS.r_close(R);
	}
	bool res;

	if (IsLtx)
		res = Scene->LoadLTX(TCHAR_TO_ANSI(*FileName), false);
	else
		res = Scene->Load(TCHAR_TO_ANSI(*FileName), false);
	if (!res)
	{
		Scene = nullptr;
		return false;
	}
	if (LevelImportOptions.ImportStaticMeshes)
	{
		for (TSharedPtr<FXRayCustomObject>& Object : Scene->GetOTool(ERBMKSceneObjectType::SceneObject)->Objects)
		{
			CSceneObject* SceneObject = reinterpret_cast<CSceneObject*>(Object->QueryInterface(ERBMKSceneObjectType::SceneObject));
			CEditableObject* EditableObject = SceneObject->GetReference();
			if (EditableObject)
			{
				bool UseOnlyCollision = false;
				UStaticMesh* StaticMesh = EngineFactory.ImportObjectAsStaticMesh(EditableObject, true);
				if (StaticMesh)
				{
					SceneObject->UpdateTransform();
					Fquaternion XRayQuat;
					XRayQuat.set(SceneObject->FTransformR);
					FQuat Quat(XRayQuat.x, -XRayQuat.z, -XRayQuat.y, XRayQuat.w);
					FVector Location(-SceneObject->GetPosition().x * 100, SceneObject->GetPosition().z * 100, SceneObject->GetPosition().y * 100);
					FRotator Rotation(Quat);
					FVector Scale3D(SceneObject->GetScale().x, SceneObject->GetScale().z, SceneObject->GetScale().y);

					AStaticMeshActor* StaticMeshActor = World->SpawnActor<AStaticMeshActor>(Location, Rotation);
					StaticMeshActor->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
					StaticMeshActor->SetActorScale3D(Scale3D);
					StaticMeshActor->SetFolderPath(TEXT("StaticMeshes"));
					FString Label = EditableObject->GetName();
					Label.ReplaceCharInline(TEXT('\\'), TEXT('/'));
					StaticMeshActor->SetActorLabel(Label);
					if(UStalkerStaticMeshAssetUserData* StalkerStaticMeshAssetUserData = StaticMesh->GetAssetUserData<UStalkerStaticMeshAssetUserData>())
					{
						StaticMeshActor->GetStaticMeshComponent()->SetVisibility(!StalkerStaticMeshAssetUserData->IsOnlyCollision);
					}
				}
			}
		}
	}
	if (LevelImportOptions.ImportParticles)
	{
		for (TSharedPtr<FXRayCustomObject>& Object : Scene->GetOTool(ERBMKSceneObjectType::ParticleSystem)->Objects)
		{
			CParticlesObject* PSObject = reinterpret_cast<CParticlesObject*>(Object->QueryInterface(ERBMKSceneObjectType::ParticleSystem));
			FString Name =  PSObject->m_RefName.c_str();
			UNiagaraSystem * NiagaraSystem = nullptr;
			{
				Name.ReplaceCharInline(TEXT('\\'), TEXT('/'));
				switch (xrGameManager::GetGame())
				{
				default:
				{
					const FString ParentPackageName = TEXT("/Game/COP/Particles") / Name;
					const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
					NiagaraSystem = LoadObject<UNiagaraSystem>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
				}
				break;
				case EGame::SHOC:
				{
					const FString ParentPackageName = TEXT("/Game/SHOC/Particles") / Name;
					const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
					NiagaraSystem = LoadObject<UNiagaraSystem>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
				}
				break;
				case EGame::CS:
				{
					const FString ParentPackageName = TEXT("/Game/CS/Particles") / Name;
					const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
					NiagaraSystem = LoadObject<UNiagaraSystem>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
				}
				break;
				}
			}
			if (NiagaraSystem)
			{
				PSObject->UpdateTransform();
				Fquaternion XRayQuat;
				XRayQuat.set(PSObject->FTransformR);
				FQuat Quat(XRayQuat.x, -XRayQuat.z, -XRayQuat.y, XRayQuat.w);
				FVector Location(-PSObject->GetPosition().x * 100, PSObject->GetPosition().z * 100, PSObject->GetPosition().y * 100);
				FRotator Rotation(Quat);

				ANiagaraActor* NiagaraActor = World->SpawnActor<ANiagaraActor>(Location, Rotation);
				NiagaraActor->GetNiagaraComponent()->SetAsset(NiagaraSystem);
				NiagaraActor->SetFolderPath(TEXT("Particles"));
				FString Label = PSObject->GetName();
				Label.ReplaceCharInline(TEXT('\\'), TEXT('/'));
				NiagaraActor->SetActorLabel(Label);
			}
		}
	}
	if (LevelImportOptions.ImportWayObjects)
	{
		for (TSharedPtr<FXRayCustomObject>& Object : Scene->GetOTool(ERBMKSceneObjectType::Way)->Objects)
		{
			CWayObject* WayObject = reinterpret_cast<CWayObject*>(Object->QueryInterface(ERBMKSceneObjectType::Way));
			if (WayObject&& WayObject->m_WayPoints.size())
			{
				AStalkerWayObject* WayObjectActor = World->SpawnActor<AStalkerWayObject>(FVector(StalkerMath::XRayLocationToUnreal( WayObject->m_WayPoints[0]->m_vPosition)),FRotator(0,0,0));
				WayObjectActor->SetFolderPath(TEXT("Ways"));
				for (int32 i = 0; i < WayObject->m_WayPoints.size()-1; i++)
				{
					WayObjectActor->CreatePoint(FVector(StalkerMath::XRayLocationToUnreal(WayObject->m_WayPoints[0]->m_vPosition)),false);
				}
				for (int32 i = 0; i < WayObject->m_WayPoints.size() ; i++)
				{
					WayObjectActor->Points[i]->SetWorldLocation(FVector(StalkerMath::XRayLocationToUnreal(WayObject->m_WayPoints[i]->m_vPosition)));
					WayObjectActor->Points[i]->PointName = WayObject->m_WayPoints[i]->m_Name.c_str();
					WayObjectActor->Points[i]->Flags = WayObject->m_WayPoints[i]->m_Flags.get();
					for (SWPLink*Link:WayObject->m_WayPoints[i]->m_Links)
					{
						auto Iterator =  std::find_if(WayObject->m_WayPoints.begin(), WayObject->m_WayPoints.end(),[Link](CWayPoint*Left){return Left==Link->way_point;});
						check(Iterator != WayObject->m_WayPoints.end());
						int32 Id = Iterator- WayObject->m_WayPoints.begin();
						WayObjectActor->Points[i]->AddLink(WayObjectActor->Points[Id], Link->probability);
					}
				}
				WayObjectActor->SetActorLabel(WayObject->GetName());
			}
		}
	}
	if (LevelImportOptions.ImportSpawnObjects)
	{
		for (TSharedPtr<FXRayCustomObject>& Object : Scene->GetOTool(ERBMKSceneObjectType::SpawnPoint)->Objects)
		{
			CSpawnPoint* SpawnObject = reinterpret_cast<CSpawnPoint*>(Object->QueryInterface(ERBMKSceneObjectType::SpawnPoint));
			if (SpawnObject && SpawnObject->m_SpawnData.m_Data)
			{
				SpawnObject->UpdateTransform();
				Fquaternion XRayQuat;
				XRayQuat.set(SpawnObject->FTransformR);
				FQuat Quat(XRayQuat.x, -XRayQuat.z, -XRayQuat.y, XRayQuat.w);
				FVector Location(-SpawnObject->GetPosition().x * 100, SpawnObject->GetPosition().z * 100, SpawnObject->GetPosition().y * 100);
				FRotator Rotation(Quat);
				FVector Scale3D(SpawnObject->GetScale().x, SpawnObject->GetScale().z, SpawnObject->GetScale().y);
				AStalkerSpawnObject* SpawnObjectActor = World->SpawnActor<AStalkerSpawnObject>(Location, Rotation);
				SpawnObjectActor->SetFolderPath(TEXT("Spawns"));
				SpawnObjectActor->SetActorScale3D(Scale3D);
				SpawnObjectActor->DestroyEntity();
				SpawnObjectActor->SetActorLabel(SpawnObject->GetName());
				{
					SpawnObjectActor->SectionName = SpawnObject->m_SpawnData.m_Data->name();
					SpawnObjectActor->EntityData.Reset();
					NET_Packet 			Packet;
					SpawnObject->m_SpawnData.m_Data->Spawn_Write(Packet, TRUE);
					for (u32 i = 0; i < Packet.B.count; i++)
					{
						SpawnObjectActor->EntityData.Add(Packet.B.data[i]);
					}
				}
				SpawnObjectActor->SpawnRead();

				if (SpawnObject->m_AttachedObject)
				{
					CEditShape* Shape = reinterpret_cast<CEditShape*>(SpawnObject->m_AttachedObject->QueryInterface(ERBMKSceneObjectType::Shape));
					Shape->UpdateTransform();
					if (Shape)
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
								ShapeTransform.SetLocation(FVector(StalkerMath::XRayLocationToUnreal(ShapeData.data.sphere.P) + StalkerMath::XRayLocationToUnreal(Shape->FTransform.c)));
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
								MBox.mulA_43(Shape->FTransform);

								FTransform ShapeTransform;
								ShapeTransform.SetLocation(FVector(StalkerMath::XRayLocationToUnreal(MBox.c)));
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
									FVector UnrealTangetX = FVector(StalkerMath::XRayNormalToUnreal(TangetX));
									FVector UnrealTangetZ = FVector(StalkerMath::XRayNormalToUnreal(TangetY));
									FVector UnrealTangetY = FVector(StalkerMath::XRayNormalToUnreal(TangetZ));

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
			XrGameMaterialLibraryInterface* LastGameMaterialLibrary=GameMaterialLibrary;
			Library.Load();
			GameMaterialLibrary = &Library;
			auto build_mesh = [] (const Fmatrix& parent, CEditableMesh* mesh, CGeomPartExtractor* extractor, u32 game_mtl_mask)
			{
				bool bResult 			= true;
				mesh->GenerateVNormals	(&parent);
				// fill faces
				for (SurfFaces::const_iterator sp_it=mesh->GetSurfFaces().begin(); sp_it!=mesh->GetSurfFaces().end(); sp_it++){
					const IntVec& face_lst 	= sp_it->second;
					CSurface* surf 		= sp_it->first;
					int gm_id			= surf->_GameMtl(); 
					if (gm_id==GAMEMTL_NONE_ID){ 
        				ELog.DlgMsg		(mtError,"Object '%s', surface '%s' contain invalid game material.",mesh->Parent()->m_LibName.c_str(),surf->_Name());
        				bResult 		= FALSE; 
						break; 
					}
					SGameMtl* M 		=  GameMaterialLibrary->GetMaterialByID(gm_id);
					if (0==M){
        				ELog.DlgMsg		(mtError,"Object '%s', surface '%s' contain undefined game material.",mesh->Parent()->m_LibName.c_str(),surf->_Name());
        				bResult 		= FALSE; 
						break; 
					}
					if (!M->Flags.is(game_mtl_mask)) continue;

					const st_Face* faces	= mesh->GetFaces();        	VERIFY(faces);
					const Fvector*	vn	 	= mesh->GetVNormals();		VERIFY(vn);
					const Fvector*	pts 	= mesh->GetVertices();		VERIFY(pts);
					for (IntVec::const_iterator f_it=face_lst.begin(); f_it!=face_lst.end(); f_it++){
						const st_Face& face = faces[*f_it];
						Fvector 		v[3],n[3];
						parent.transform_tiny	(v[0],pts[face.pv[0].pindex]);
						parent.transform_tiny	(v[1],pts[face.pv[1].pindex]);
						parent.transform_tiny	(v[2],pts[face.pv[2].pindex]);
						parent.transform_dir	(n[0],vn[*f_it*3+0]); n[0].normalize();
						parent.transform_dir	(n[1],vn[*f_it*3+1]); n[1].normalize();
						parent.transform_dir	(n[2],vn[*f_it*3+2]); n[2].normalize();
						const Fvector2*	uv[3];
						mesh->GetFaceTC			(*f_it,uv);
						extractor->AppendFace	(surf,v,n,uv);
					}
					if (!bResult) break;
				}
				mesh->UnloadVNormals	();
				return bResult;
			};

			auto OrientToNorm = [] (Fvector& local_norm, Fmatrix33& form, Fvector& hs)
			{
				Fvector* ax_pointer = (Fvector*)&form;
				int 	max_proj = 0, min_size = 0;
				for (u32 k = 1; k < 3; k++) {
					if (_abs(local_norm[k]) > _abs(local_norm[max_proj]))
						max_proj = k;
					if (hs[k] < hs[min_size])
						min_size = k;
				}
				if (min_size != max_proj) return false;
				if (local_norm[max_proj] < 0.f) {
					local_norm.invert();
					ax_pointer[max_proj].invert();
					ax_pointer[(max_proj + 1) % 3].invert();
				}
				return true;
			};

			CGeomPartExtractor* extractor   = 0;
			ESceneObjectTool* ObjectTool = static_cast<ESceneObjectTool*>(Scene->GetTool(ERBMKSceneObjectType::SceneObject));
			Fbox 		bb;

			extractor	                    = new CGeomPartExtractor;
			extractor->Initialize           (bb,EPS_L,int_max);

			// collect verts&&faces
			{
				for (TSharedPtr<FXRayCustomObject>& Object : ObjectTool->Objects)
				{
					CSceneObject* obj 		= reinterpret_cast<CSceneObject*>(Object->QueryInterface(ERBMKSceneObjectType::SceneObject));
					VERIFY                  (obj);
					if (obj->IsStatic())
					{
						CEditableObject *O 	= obj->GetReference();
						obj->UpdateTransform();
						const Fmatrix& T 	= obj->FTransform;
                
						for(EditMeshIt M =O->FirstMesh(); M!=O->LastMesh(); M++)
						{
							if (!build_mesh	(T, *M, extractor, SGameMtl::flClimable))
							{
							  break;
							}
						}
					
					}
				}
			}
			extractor->Process();
			{
    			SBPartVec& parts			= extractor->GetParts();
				int32 Index = 0;
				for (SBPartVecIt p_it=parts.begin(); p_it!=parts.end(); p_it++)
				{
					SBPart*	P				= *p_it;
        			if (P->Valid())
					{
						Fvector local_normal	        = {0,0,0};

						LPCSTR mat_name = NULL;
						for (SBFaceVecIt it=P->m_Faces.begin(); it!=P->m_Faces.end(); it++)
						{
                			for (u32 k=0; k<3; k++)
								local_normal.add	        ((*it)->n[k]);

							mat_name     = (*it)->surf->_GameMtlName();
						}

						local_normal.normalize_safe		();
						// orientate object
	          			if (!OrientToNorm(local_normal,P->m_OBB.m_rotate,P->m_OBB.m_halfsize))
						{
                    		continue;
						}
						else
						{

							Fmatrix M; M.set			(P->m_OBB.m_rotate.i,P->m_OBB.m_rotate.j,P->m_OBB.m_rotate.k,P->m_OBB.m_translate);
							FMatrix InM = StalkerMath::XRayMatrixToUnreal(M);
							AStalkerSpawnObject* SpawnObjectActor = World->SpawnActor<AStalkerSpawnObject>();
							SpawnObjectActor->SetActorTransform(FTransform(InM));
							SpawnObjectActor->SetFolderPath(TEXT("Spawns"));
							SpawnObjectActor->DestroyEntity();
							SpawnObjectActor->SetActorLabel(FString::Printf(TEXT("clmbl#%d"),Index));
							SpawnObjectActor->SectionName = "climable_object";
							SpawnObjectActor->SpawnRead();
							{
								UStalkerSpawnObjectBoxShapeComponent* NewShapeComponent = NewObject< UStalkerSpawnObjectBoxShapeComponent>(SpawnObjectActor, NAME_None, RF_Transactional);
								SpawnObjectActor->AddInstanceComponent(NewShapeComponent);
								NewShapeComponent->AttachToComponent(SpawnObjectActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
								NewShapeComponent->OnComponentCreated();
								NewShapeComponent->RegisterComponent();
								NewShapeComponent->BoxExtent.Set((P->m_BBox.max.x-P->m_BBox.min.x)*50,(P->m_BBox.max.z-P->m_BBox.min.z)*50,(P->m_BBox.max.y-P->m_BBox.min.y)*50);
							}
						}
					}
					else
					{
            			ELog.Msg(mtError,"Can't export invalid part #%d",p_it-parts.begin());
					}
				}
			}
			// clean up
			delete extractor;
			GameMaterialLibrary = LastGameMaterialLibrary;
		}
	}
	if (LevelImportOptions.ImportAIMap)
	{
		AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
		if (StalkerWorldSettings)
		{
			if(UStalkerAIMap* INAIMap = StalkerWorldSettings->GetOrCreateAIMap())
			{
				ESceneAIMapTool* AIMapTool = static_cast<ESceneAIMapTool*>(Scene->GetTool(ERBMKSceneObjectType::AIMap));
				INAIMap->ClearAIMap();
				if (AIMapTool)
				{
					INAIMap->Nodes.AddDefaulted(AIMapTool->m_Nodes.size());
					for (int32 i = 0; i < AIMapTool->m_Nodes.size(); i++)
					{
						INAIMap->Nodes[i] = new FStalkerAIMapNode;
					}
					for (int32 i = 0; i < AIMapTool->m_Nodes.size(); i++)
					{
						INAIMap->Nodes[i]->Position = StalkerMath::XRayLocationToUnreal(AIMapTool->m_Nodes[i]->Pos);
						INAIMap->Nodes[i]->Position.X = INAIMap->NodeSize * FMath::RoundToDouble(INAIMap->Nodes[i]->Position.X / INAIMap->NodeSize);
						INAIMap->Nodes[i]->Position.Y = INAIMap->NodeSize * FMath::RoundToDouble(INAIMap->Nodes[i]->Position.Y / INAIMap->NodeSize);
						FVector3f PlaneNormal = StalkerMath::XRayNormalToUnreal(AIMapTool->m_Nodes[i]->Plane.n);
						INAIMap->Nodes[i]->Plane.X = PlaneNormal.X;
						INAIMap->Nodes[i]->Plane.Y = PlaneNormal.Y;
						INAIMap->Nodes[i]->Plane.Z = PlaneNormal.Z;
						INAIMap->Nodes[i]->Plane.W = -AIMapTool->m_Nodes[i]->Plane.d * 100.f;
						for (int32 Link = 0; Link < 4; Link++)
						{
							if (AIMapTool->m_Nodes[i]->n[Link])
							{
								INAIMap->Nodes[i]->Nodes[Link] = INAIMap->Nodes[AIMapTool->m_Nodes[i]->n[Link]->idx];
							}
						}
					}
				}
				INAIMap->HashFill();
				INAIMap->Modify();
			}
		}
	}
	if (LevelImportOptions.ImportWallmark)
	{
		if(ESceneWallmarkTool* WallmarkTool = static_cast<ESceneWallmarkTool*>(Scene->GetTool(ERBMKSceneObjectType::Wallmark)))
		{
			for( ESceneWallmarkTool::wm_slot*slot:WallmarkTool->marks)
			{
				for(ESceneWallmarkTool::wallmark* item: slot->items)
				{
					Fvector XRayPosition,XRaySize;
					item->bbox.getcenter(XRayPosition);
					item->bbox.getsize(XRaySize);
					FVector Location(StalkerMath::XRayLocationToUnreal(XRayPosition));
					FPlane4f WMPlane = FPlane4f(StalkerMath::XRayLocationToUnreal(item->verts[0].p),StalkerMath::XRayLocationToUnreal(item->verts[1].p),StalkerMath::XRayLocationToUnreal(item->verts[2].p));
					;
					FQuat Rotation = FQuat(WMPlane.ToOrientationQuat())*FRotator(-90,0,0).Quaternion();
					Rotation = Rotation*FRotator(0,FMath::RadiansToDegrees(item->r)-90,0).Quaternion();
					ADecalActor* DecalActor = World->SpawnActor<ADecalActor>(Location, Rotation.Rotator());
					DecalActor->SetFolderPath(TEXT("Decals"));
					DecalActor->GetDecal()->DecalSize = FVector(FMath::Max(item->w,item->h)*12.5f,item->h*50,item->w*50);
					DecalActor->SetDecalMaterial(ImportSurfaceForDecal(slot->sh_name,slot->tx_name));
				}
			}
		}
	}
	if (LevelImportOptions.ImportDetail)
	{
		if(FRBMKSceneDetailObjectTool* DOSceneTool = static_cast<FRBMKSceneDetailObjectTool*>(Scene->GetTool(ERBMKSceneObjectType::DetailObject)))
		{
			DOSceneTool->DetailDensity = LevelImportOptions.DetailDensity;
			DOSceneTool->ExportToCurrentWorld();
		}
	}
	Scene = nullptr;
	return true;
}

UMaterialInterface* RBMKLevelFactory::ImportSurfaceForDecal(shared_str ShaderName, shared_str TextureName)
{
	if (ShaderName.size() == 0)
		return nullptr;
			
	
	FString ParentName = FString(ShaderName.c_str()).Replace(TEXT("\\"), TEXT("/"));
	FString GlobalMaterialInstanceName =  UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("MaterialsInstance") / ParentName/( FPaths::ChangeExtension(TextureName.c_str(), TEXT("")).Replace(TEXT("\\"), TEXT("/"))));

	UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, *(GlobalMaterialInstanceName + TEXT(".") + FPaths::GetBaseFilename(GlobalMaterialInstanceName)), nullptr, LOAD_NoWarn);
	if (Material)
		return Material;

	UMaterialInterface* ParentMaterial = nullptr;
	{
		const FString ParentPackageName = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Materials") / ParentName);
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	if (!IsValid(ParentMaterial))
	{
		const FString ParentPackageName = UPackageTools::SanitizePackageName(TEXT("/Game/Base/Materials") / ParentName);
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	if (!IsValid(ParentMaterial))
	{
		UMaterialInterface* UnkownMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Base/Materials/Decals/Unkown.Unkown"));
		check(IsValid(UnkownMaterial));
		const FString ParentPackageName = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Materials") / ParentName);

		UPackage* AssetPackage = CreatePackage(*ParentPackageName);
		UMaterialInstanceConstant* NewParentMaterial = NewObject<UMaterialInstanceConstant>(AssetPackage, *FPaths::GetBaseFilename(ParentPackageName), RF_Standalone|RF_Public);
		NewParentMaterial->Parent = UnkownMaterial;
		FAssetRegistryModule::AssetCreated(NewParentMaterial);
		NewParentMaterial->Modify();
		NewParentMaterial->PostEditChange();
		ParentMaterial = NewParentMaterial;
	}
	UPackage* AssetPackage = CreatePackage(*GlobalMaterialInstanceName);
	UMaterialInstanceConstant* NewMaterial = NewObject<UMaterialInstanceConstant>(AssetPackage, *FPaths::GetBaseFilename(GlobalMaterialInstanceName), RF_Standalone|RF_Public);
	FAssetRegistryModule::AssetCreated(NewMaterial);
	NewMaterial->Parent = ParentMaterial;

	FStaticParameterSet NewStaticParameterSet;
	TObjectPtr<UTexture2D> BaseTexture = EngineFactory.ImportTexture(TextureName.c_str());
	static FName NAME_Diffuse = "Diffuse";
	if (BaseTexture)
	{
		NewMaterial->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(NAME_Diffuse), BaseTexture);
	}

	
	NewMaterial->UpdateStaticPermutation(NewStaticParameterSet);
	NewMaterial->InitStaticPermutation();
	NewMaterial->Modify();
	NewMaterial->PostEditChange();
	return NewMaterial;
}
