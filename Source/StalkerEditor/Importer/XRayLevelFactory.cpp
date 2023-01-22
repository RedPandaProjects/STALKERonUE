#include "XRayLevelFactory.h"
#include "Scene/Entitys/StaticObject/SceneObject.h"
#include "Scene/Tools/AIMap/ESceneAIMapTools.h"
#include "Kernel/Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/AIMap/StalkerAIMap.h"
#include "Scene/Entitys/WayObject/WayPoint.h"
#include "../Entities/AI/WayObject/StalkerWayObject.h"
#include "../Entities/AI/WayObject/StalkerWayPointComponent.h"

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
	}
	Scene = nullptr;
	return true;
}