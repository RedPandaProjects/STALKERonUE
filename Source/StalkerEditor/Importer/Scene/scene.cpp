#include "scene.h"
#include "Tools/StaticObject/ESceneObjectTools.h"
#include "Tools/Base/ESceneDummyTools.h"
#include "Tools/AIMap/ESceneAIMapTools.h"
#include "Tools/WayPoint/ESceneWayTools.h"
#include "Tools/Shape/ESceneShapeTools.h"
#include "Tools/Spawn/ESceneSpawnTools.h"
#include "Tools/Group/ESceneGroupTools.h"
#include "Tools/Particles/EScenePSTools.h"
#include "Tools/Wallmark/ESceneWallmarkTool.h"
#include "Entitys/GroupObject/GroupObject.h"
#include "Tools/Detail/RBMKSceneDetailObjectTool.h"


EScene* Scene;

st_LevelOptions::st_LevelOptions()
{
	Reset();
}

void st_LevelOptions::Reset()
{
	m_FNLevelPath		= "level";
    m_LevelPrefix		= "level_prefix";
	m_BOPText			= "";
	m_map_version		= "1.0";
	m_mapUsage.SetDefaults	();
}

EScene::EScene()
{
    for (int32 i = 0; i < static_cast<int32>(ERBMKSceneObjectType::Count); i++)
    {
    	SceneTools.Add(static_cast<ERBMKSceneObjectType>(i));
    }
	CreateSceneTools();
}

EScene::~EScene()
{
	DestroySceneTools();
}


void EScene::AppendObject( FXRayCustomObject* object, bool bUndo )
{
	VERIFY			  	(object);
	VERIFY				(m_Valid);
    FXRaySceneCustomOTool* Mt	= GetOTool(object->FClassID);
    Mt->_AppendObject	(object);
}

bool EScene::RemoveObject( FXRayCustomObject* object, bool bUndo, bool bDeleting )
{
	VERIFY				(object);
	VERIFY				(m_Valid);

    FXRaySceneCustomOTool* mt 	= GetOTool(object->FClassID);
    if (mt)
    {
    	mt->_RemoveObject(object);
    }
    return true;
}


void EScene::Clear()
{
	for(auto&[Key,Value]:SceneTools)
	{
		if(Key!=ERBMKSceneObjectType::AllTypes&&Value)
		{
			Value->Clear();
		}
	}
}



xr_string EScene::LevelPath()
{
    string_path path;
	if (m_LevelOp.m_FNLevelPath.size()){
        FS.update_path	(path,"$level$",m_LevelOp.m_FNLevelPath.c_str());
        FCStringAnsi::Strcat(path,"\\");
    }
    return xr_string(path);
}


FXRayCustomObject* EScene::FindObjectByName(LPCSTR name, ERBMKSceneObjectType classfilter)
{
	if (!name)
	{
		return nullptr;
	}

	FXRayCustomObject* Result = nullptr;
	if (classfilter == ERBMKSceneObjectType::AllTypes)
	{
		for(auto&[Key,Value]:SceneTools)
		{
			if(!Value)
			{
				continue;
			}

			if(FXRaySceneCustomOTool* SceneCustomOTool = Value->CastToSceneCustomOTool())
			{
				Result = SceneCustomOTool->FindObjectByName(name);
				if(Result)
				{
					return Result;
				}
			}
		}
	}
	else 
	{
		if(FXRaySceneCustomOTool* SceneCustomOTool = GetOTool(classfilter))
		{
			Result = SceneCustomOTool->FindObjectByName(name);
			if(Result)
			{
				return Result;
			}
		}
	}
	return Result;
}


FXRayCustomObject* EScene::FindObjectByName(LPCSTR name, FXRayCustomObject* pass_object)
{
	FXRayCustomObject* Result = 0;
	for(auto&[Key,Value]:SceneTools)
	{
		if(!Value)
		{
			continue;
		}

		if(FXRaySceneCustomOTool* SceneCustomOTool = Value->CastToSceneCustomOTool())
		{
			Result = SceneCustomOTool->FindObjectByName(name,pass_object);
			if(Result)
			{
				return Result;
			}
		}
	}
	return nullptr;
}


void EScene::GenObjectName(ERBMKSceneObjectType cls_id, char* buffer, const char* pref)
{


    for (int i = 0; true; i++)
    {
        bool result;
        xr_string temp;
        if (pref)
        {
            if (i == 0)
            {
                temp = pref;
            }
            else
            {
                temp.Printf("%s_%02d", pref, i - 1);
            }
        }
        else
        {
            temp.Printf("%02d", i);
        }

        FindObjectByNameCB(temp.c_str(), result);
        if (!result)
        {
            xr_strcpy(buffer, 256, temp.c_str());
            return;
        }
    }
}

void EScene::RegisterSceneTools(TSharedPtr<FXRaySceneToolBase> SceneToolBase)
{
	SceneTools.FindOrAdd(SceneToolBase->FClassID) = SceneToolBase;
}

void EScene::CreateSceneTools()
{
	RegisterSceneTools(MakeShared<ESceneDummyTool>() );
	RegisterSceneTools(MakeShared<ESceneObjectTool>());
	RegisterSceneTools(MakeShared<ESceneAIMapTool>());
	RegisterSceneTools(MakeShared<ESceneWayTool>()); 
	RegisterSceneTools(MakeShared<ESceneShapeTool>());
	RegisterSceneTools(MakeShared<ESceneSpawnTool>());
	RegisterSceneTools(MakeShared<ESceneGroupTool>()); 
	RegisterSceneTools(MakeShared<EScenePSTools>());
	RegisterSceneTools(MakeShared<ESceneWallmarkTool>());
	RegisterSceneTools(MakeShared<FRBMKSceneDetailObjectTool>());
}

void EScene::DestroySceneTools()
{
	SceneTools.Empty();
}


bool EScene::GetSubstObjectName(const xr_string& _from, xr_string& _to) const
{
    return false;
}

void EScene::GetObjects(ERBMKSceneObjectType InType, ObjectList& OutObjects)
{
	ObjectList&ObjectsFromTool = GetOTool(InType)->GetObjects();
	for (FXRayCustomObject* Object : ObjectsFromTool)
	{
		OutObjects.push_back(Object);
	}
	/*for (auto& GroupObj : ListObj(ERBMKSceneObjectType::Group))
	{
		ObjectList ObjectsFromGroups;
		CGroupObject* Group = reinterpret_cast<CGroupObject*>(GroupObj->QueryInterface(ERBMKSceneObjectType::Group));
		Group->GetObjects(ObjectsFromGroups);
		for (auto& Object : ObjectsFromGroups)
		{
			if (GroupObj->FClassID == InType)
			{
				OutObjects.push_back(Object);
			}
		}
	}*/
}

int EScene::ObjCount()
{
	int32 Counter = 0;

	for(auto&[Key,Value]:SceneTools)
	{
		if(!Value)
		{
			continue;
		}

		if(FXRaySceneCustomOTool* SceneCustomOTool = Value->CastToSceneCustomOTool())
		{
			Counter += SceneCustomOTool->ObjCount();
		}
	}

	return Counter;
}