#include "scene.h"
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


void EScene::AppendObject(TSharedPtr<FXRayCustomObject> Object )
{
	if(ensure(Object))
	{
		FRBMKSceneObjectsToolBase* Mt	= GetOTool(Object->FClassID);
		Mt->Objects.Add(Object);
	}
}

bool EScene::RemoveObject(TSharedPtr<FXRayCustomObject> Object )
{
    FRBMKSceneObjectsToolBase* mt 	= GetOTool(Object->FClassID);
    if (mt&&Object)
    {
    	mt->Objects.Remove(Object);
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


TSharedPtr<FXRayCustomObject>  EScene::FindObjectByName(LPCSTR name, ERBMKSceneObjectType classfilter)
{
	if (!name)
	{
		return nullptr;
	}

	TSharedPtr<FXRayCustomObject> Result = nullptr;
	if (classfilter == ERBMKSceneObjectType::AllTypes)
	{
		for(auto&[Key,Value]:SceneTools)
		{
			if(!Value)
			{
				continue;
			}

			if(FRBMKSceneObjectsToolBase* SceneCustomOTool = Value->CastToSceneCustomOTool())
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
		if(FRBMKSceneObjectsToolBase* SceneCustomOTool = GetOTool(classfilter))
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


TSharedPtr<FXRayCustomObject>  EScene::FindObjectByName(LPCSTR name)
{
	TSharedPtr<FXRayCustomObject>  Result = 0;
	for(auto&[Key,Value]:SceneTools)
	{
		if(!Value)
		{
			continue;
		}

		if(FRBMKSceneObjectsToolBase* SceneCustomOTool = Value->CastToSceneCustomOTool())
		{
			Result = SceneCustomOTool->FindObjectByName(name);
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
		result = FindObjectByName(temp.c_str()).IsValid();
        if (!result)
        {
            xr_strcpy(buffer, 256, temp.c_str());
            return;
        }
    }
}

void EScene::RegisterSceneTools(TSharedPtr<FRBMKSceneToolBase> SceneToolBase)
{
	SceneTools.FindOrAdd(SceneToolBase->ObjectType) = SceneToolBase;
}

void EScene::CreateSceneTools()
{
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

int EScene::ObjCount()
{
	int32 Counter = 0;

	for(auto&[Key,Value]:SceneTools)
	{
		if(!Value)
		{
			continue;
		}

		if(FRBMKSceneObjectsToolBase* SceneCustomOTool = Value->CastToSceneCustomOTool())
		{
			Counter += SceneCustomOTool->Objects.Num();
		}
	}

	return Counter;
}