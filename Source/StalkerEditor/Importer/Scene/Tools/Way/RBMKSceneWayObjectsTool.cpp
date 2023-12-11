#include "RBMKSceneWayObjectsTool.h"

#include "Entities/Scene/WayObject/StalkerWayObject.h"
#include "Entities/Scene/WayObject/StalkerWayPointComponent.h"
#include "Importer/Scene/Entitys/WayObject/RBMKSceneWayObject.h"

FRBMKSceneWayObjectsTool::FRBMKSceneWayObjectsTool():FRBMKSceneObjectsToolBase(ERBMKSceneObjectType::Way)
{
}

FRBMKSceneWayObjectsTool::~FRBMKSceneWayObjectsTool()
{

}

TSharedPtr<FRBMKSceneObjectBase> FRBMKSceneWayObjectsTool::CreateObject(const FString&Name)
{
    return MakeShared<FRBMKSceneWayObject>(Name);
}

bool FRBMKSceneWayObjectsTool::LoadLTX(CInifile& ini)
{
	uint32 Version 	= ini.r_u32("main","version");
    if(Version != ToolsVersion )
    {
    	return false;
    }
	FRBMKSceneObjectsToolBase::LoadLTX(ini);
	return true;
}

bool FRBMKSceneWayObjectsTool::LoadStream(IReader& F)
{
	uint16 Version 	= 0;
    if(F.r_chunk(ChunkVersion,&Version))
    {
    	if( Version!=ToolsVersion )
	    {
	        return false;
	    }
    }
	return FRBMKSceneObjectsToolBase::LoadStream(F);
}

bool FRBMKSceneWayObjectsTool::LoadSelection(IReader& F)
{
	uint16 Version 	= 0;
    if(F.r_chunk(ChunkVersion,&Version))
    {
    	if( Version!=ToolsVersion )
	    {
	        return false;
	    }
    }
	return FRBMKSceneObjectsToolBase::LoadSelection(F);
}

void FRBMKSceneWayObjectsTool::ExportToWorld(UWorld* World, EObjectFlags InFlags,const URBMKLevelImportOptions&LevelImportOptions)
{
	for (TSharedPtr<FRBMKSceneObjectBase>& Object : Objects)
	{
		FRBMKSceneWayObject* WayObject = reinterpret_cast<FRBMKSceneWayObject*>(Object->QueryInterface(ERBMKSceneObjectType::Way));
		if (WayObject&& WayObject->WayPoints.Num())
		{

			AStalkerWayObject* WayObjectActor = World->SpawnActor<AStalkerWayObject>(WayObject->WayPoints[0]->Position,FRotator::ZeroRotator);
			WayObjectActor->SetFolderPath(TEXT("Ways"));
			for (int32 i = 0; i < WayObject->WayPoints.Num()-1; i++)
			{
				WayObjectActor->CreatePoint(WayObject->WayPoints[0]->Position,false);
			}
			for (int32 i = 0; i < WayObject->WayPoints.Num() ; i++)
			{
				WayObjectActor->Points[i]->SetWorldLocation(WayObject->WayPoints[i]->Position);
				WayObjectActor->Points[i]->PointName = WayObject->WayPoints[i]->Name;
				WayObjectActor->Points[i]->Flags = WayObject->WayPoints[i]->Flags;
				for (FRBMKSceneWayPointLink& Link :WayObject->WayPoints[i]->Links)
				{
					int32 Index = WayObject->WayPoints.IndexOfByKey(Link.WayPoint);
					check(Index != INDEX_NONE);
					WayObjectActor->Points[i]->AddLink(WayObjectActor->Points[Index], Link.Probability);
				}
			}
			WayObjectActor->SetActorLabel(WayObject->GetName());
		}
	}
}
