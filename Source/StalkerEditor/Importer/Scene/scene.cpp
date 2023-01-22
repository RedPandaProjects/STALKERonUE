#include "scene.h"
#include "Tools/StaticObject/ESceneObjectTools.h"
#include "Tools/Base/ESceneDummyTools.h"
#include "Tools/AIMap/ESceneAIMapTools.h"
#include "Tools/WayPoint/ESceneWayTools.h"


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
    for (int i=0; i<OBJCLASS_COUNT; i++)
        m_SceneTools.insert(mk_pair((ObjClassID)i,(ESceneToolBase*)NULL));
	CreateSceneTools();
}

EScene::~EScene()
{
	DestroySceneTools();
}


void EScene::AppendObject( CCustomObject* object, bool bUndo )
{
	VERIFY			  	(object);
	VERIFY				(m_Valid);
    ESceneCustomOTool* Mt	= GetOTool(object->FClassID);
    Mt->_AppendObject	(object);
}

bool EScene::RemoveObject( CCustomObject* object, bool bUndo, bool bDeleting )
{
	VERIFY				(object);
	VERIFY				(m_Valid);

    ESceneCustomOTool* mt 	= GetOTool(object->FClassID);
    if (mt)
    {
    	mt->_RemoveObject(object);
    }
    return true;
}


void EScene::Clear()
{
    // clear scene tools
    SceneToolsMapPairIt t_it = m_SceneTools.begin();
    SceneToolsMapPairIt t_end = m_SceneTools.end();
    for (; t_it != t_end; t_it++)
        if (t_it->second && t_it->first != OBJCLASS_DUMMY) 
        {
                t_it->second->Clear();
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


CCustomObject* EScene::FindObjectByName(LPCSTR name, ObjClassID classfilter)
{
	if (!name)
		return NULL;

	CCustomObject* object = 0;
	if (classfilter == OBJCLASS_DUMMY)
	{
		SceneToolsMapPairIt _I = m_SceneTools.begin();
		SceneToolsMapPairIt _E = m_SceneTools.end();
		for (; _I != _E; ++_I)
		{
			if (!_I->second)
			{
				continue;
			}
			ESceneCustomOTool* mt = _I->second->CastToESceneCustomOTool();

			if (mt && (0 != (object = mt->FindObjectByName(name))))
				return object;
		}
	}
	else {
		ESceneCustomOTool* mt = GetOTool(classfilter); VERIFY(mt);
		if (mt && (0 != (object = mt->FindObjectByName(name)))) return object;
	}
	return object;
}


CCustomObject* EScene::FindObjectByName(LPCSTR name, CCustomObject* pass_object)
{
	CCustomObject* object = 0;
	SceneToolsMapPairIt _I = m_SceneTools.begin();
	SceneToolsMapPairIt _E = m_SceneTools.end();
	for (; _I != _E; _I++)
	{
		if (!_I->second)
		{
			continue;
		}
		ESceneCustomOTool* mt = _I->second->CastToESceneCustomOTool();
		if (mt && (0 != (object = mt->FindObjectByName(name, pass_object)))) return object;
	}
	return 0;
}


void EScene::GenObjectName(ObjClassID cls_id, char* buffer, const char* pref)
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

void EScene::RegisterSceneTools(ESceneToolBase* mt)
{
	m_SceneTools[mt->FClassID] = mt;
}

void EScene::CreateSceneTools()
{
	RegisterSceneTools(new ESceneDummyTool); //+
	RegisterSceneTools(new ESceneObjectTool); //+
	RegisterSceneTools(new ESceneAIMapTool); //+
	RegisterSceneTools(new ESceneWayTool); //+
}

void EScene::DestroySceneTools()
{
	SceneToolsMapPairIt _I = m_SceneTools.begin();
	SceneToolsMapPairIt _E = m_SceneTools.end();
	for (; _I != _E; _I++)
	{
		if (_I->second)
		{
			delete _I->second;
			_I->second = nullptr;
		}
		
	}
	m_SceneTools.clear();
}


bool EScene::GetSubstObjectName(const xr_string& _from, xr_string& _to) const
{
    return false;
}
int EScene::ObjCount()
{
	int cnt = 0;
	SceneToolsMapPairIt _I = m_SceneTools.begin();
	SceneToolsMapPairIt _E = m_SceneTools.end();
	for (; _I != _E; _I++)
	{
		if (!_I->second)
		{
			continue;
		}
		ESceneCustomOTool* mt = _I->second->CastToESceneCustomOTool();
		if (mt)
			cnt += mt->ObjCount();
	}
	return cnt;
}