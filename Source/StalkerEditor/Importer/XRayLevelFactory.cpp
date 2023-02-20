#include "XRayLevelFactory.h"
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


XRayLevelFactory::XRayLevelFactory(UObject* InParentPackage, EObjectFlags InFlags):EngineFactory(InParentPackage, InFlags),ParentPackage(InParentPackage),ObjectFlags(InFlags)
{

}

XRayLevelFactory::~XRayLevelFactory()
{

}

bool XRayLevelFactory::ImportLevel(const FString& FileName)
{
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
	
	ObjectList&ListObj = Scene->ListObj(OBJCLASS_SCENEOBJECT);
	for(CCustomObject*Object: ListObj)
	{
		CSceneObject* SceneObject =  reinterpret_cast<CSceneObject*>(Object->QueryInterface(OBJCLASS_SCENEOBJECT));
		CEditableObject * EditableObject =  SceneObject->GetReference();
		if (EditableObject)
		{
			UStaticMesh* StaticMesh = EngineFactory.ImportObjectAsStaticMesh(EditableObject, true);
			if (StaticMesh)
			{
				SceneObject->UpdateTransform(true);
				Fquaternion XRayQuat;
				XRayQuat.set(SceneObject->FTransformR);
				FQuat Quat(XRayQuat.x, -XRayQuat.z, -XRayQuat.y, XRayQuat.w);
				FVector Location(-SceneObject->GetPosition().x * 100, SceneObject->GetPosition().z * 100, SceneObject->GetPosition().y * 100);
				FRotator Rotation(Quat);
				FVector Scale3D(SceneObject->GetScale().x, SceneObject->GetScale().z, SceneObject->GetScale().y);

				AStaticMeshActor* StaticMeshActor = World->SpawnActor<AStaticMeshActor>(Location, Rotation);
				StaticMeshActor->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
				StaticMeshActor->SetActorScale3D(Scale3D);
				FString Label = EditableObject->GetName();
				Label.ReplaceCharInline(TEXT('\\'), TEXT('/'));
				StaticMeshActor->SetActorLabel(Label);
			}
		}
	}
	ObjectList& ListWay = Scene->ListObj(OBJCLASS_WAY);
	for (CCustomObject* Object : ListWay)
	{
		CWayObject* WayObject = reinterpret_cast<CWayObject*>(Object->QueryInterface(OBJCLASS_WAY));
		if (WayObject&& WayObject->m_WayPoints.size())
		{
			AStalkerWayObject* WayObjectActor = World->SpawnActor<AStalkerWayObject>(FVector(StalkerMath::XRayLocationToUnreal( WayObject->m_WayPoints[0]->m_vPosition)),FRotator(0,0,0));
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
	ObjectList& ListSpawn = Scene->ListObj(OBJCLASS_SPAWNPOINT);
	for (CCustomObject* Object : ListSpawn)
	{
		CSpawnPoint* SpawnObject = reinterpret_cast<CSpawnPoint*>(Object->QueryInterface(OBJCLASS_SPAWNPOINT));
		if (SpawnObject && SpawnObject->m_SpawnData.m_Data)
		{
			SpawnObject->UpdateTransform(true);
			Fquaternion XRayQuat;
			XRayQuat.set(SpawnObject->FTransformR);
			FQuat Quat(XRayQuat.x, -XRayQuat.z, -XRayQuat.y, XRayQuat.w);
			FVector Location(-SpawnObject->GetPosition().x * 100, SpawnObject->GetPosition().z * 100, SpawnObject->GetPosition().y * 100);
			FRotator Rotation(Quat);
			FVector Scale3D(SpawnObject->GetScale().x, SpawnObject->GetScale().z, SpawnObject->GetScale().y);
			AStalkerSpawnObject* SpawnObjectActor = World->SpawnActor<AStalkerSpawnObject>(Location, Rotation);
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
				CEditShape* Shape = reinterpret_cast<CEditShape*>(SpawnObject->m_AttachedObject->QueryInterface(OBJCLASS_SHAPE));
				Shape->UpdateTransform(true);
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
							Fmatrix MShape = Shape->_Transform();
							ShapeTransform.SetLocation(FVector(StalkerMath::XRayLocationToUnreal(Fvector().add( MShape.c,ShapeData.data.sphere.P))));
							NewShapeComponent->SphereRadius = ShapeData.data.sphere.R*100.f;
							{
								
								Fvector A, B[8];
								A.set(-.5f, -.5f, -.5f);	MShape.transform_tiny(B[0], A);
								A.set(-.5f, -.5f, +.5f);	MShape.transform_tiny(B[1], A);
								A.set(-.5f, +.5f, +.5f);	MShape.transform_tiny(B[2], A);
								A.set(-.5f, +.5f, -.5f);	MShape.transform_tiny(B[3], A);
								A.set(+.5f, +.5f, +.5f);	MShape.transform_tiny(B[4], A);
								A.set(+.5f, +.5f, -.5f);	MShape.transform_tiny(B[5], A);
								A.set(+.5f, -.5f, +.5f);	MShape.transform_tiny(B[6], A);
								A.set(+.5f, -.5f, -.5f);	MShape.transform_tiny(B[7], A);

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
							MBox.mulA_43(Shape->_Transform());

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
							
								FMatrix MatrixRotate(UnrealTangetX, UnrealTangetY, UnrealTangetZ,FVector(0,0,0));
								ShapeTransform.SetRotation(MatrixRotate.ToQuat());

								ShapeTransform.SetScale3D(FVector(B[0].distance_to(B[7]), B[0].distance_to(B[1]) , B[0].distance_to(B[3])) / 2.f);
							}

							NewShapeComponent->SetWorldTransform(ShapeTransform);
						}
					}
				
				}

			}
		}
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (StalkerWorldSettings)
	{
		UStalkerAIMap*INAIMap =  StalkerWorldSettings->GetOrCreateAIMap();
		ESceneAIMapTool* AIMapTool = static_cast<ESceneAIMapTool*>(Scene->GetTool(OBJCLASS_AIMAP));
		INAIMap->ClearAIMap();
		if (AIMapTool)
		{
			INAIMap->Nodes.AddDefaulted(AIMapTool->m_Nodes.size());
			for (int32 i = 0; i < AIMapTool->m_Nodes.size();i++)
			{
				INAIMap->Nodes[i] = new FStalkerAIMapNode;
			}
			for (int32 i = 0; i < AIMapTool->m_Nodes.size(); i++)
			{
				INAIMap->Nodes[i]->Position = StalkerMath::XRayLocationToUnreal(AIMapTool->m_Nodes[i]->Pos);
				FVector3f PlaneNormal = StalkerMath::XRayNormalToUnreal(AIMapTool->m_Nodes[i]->Plane.n);
				INAIMap->Nodes[i]->Plane.X = PlaneNormal.X;
				INAIMap->Nodes[i]->Plane.Y = PlaneNormal.Y;
				INAIMap->Nodes[i]->Plane.Z = PlaneNormal.Z;
				INAIMap->Nodes[i]->Plane.W = -AIMapTool->m_Nodes[i]->Plane.d*100.f;
				for (int32 Link = 0; Link < 4; Link++)
				{
					if(AIMapTool->m_Nodes[i]->n[Link])
					{
						INAIMap->Nodes[i]->Nodes[Link] = INAIMap->Nodes[AIMapTool->m_Nodes[i]->n[Link]->idx];
					}
				}
			}
		}
		INAIMap->HashFill();
		INAIMap->Modify();
	}
	Scene = nullptr;
	return true;
}