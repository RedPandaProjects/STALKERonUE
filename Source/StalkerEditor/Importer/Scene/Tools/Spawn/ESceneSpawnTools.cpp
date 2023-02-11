#include "ESceneSpawnTools.h"
#include "../../Entitys/SpawnObject/SpawnPoint.h"


ESceneSpawnTool::ESceneSpawnTool():ESceneCustomOTool(OBJCLASS_SPAWNPOINT)
{
	m_Flags.zero();
}

ESceneSpawnTool::~ESceneSpawnTool()
{

}

CCustomObject* ESceneSpawnTool::CreateObject(LPVOID data, LPCSTR name)
{
	CSpawnPoint* O	= new CSpawnPoint(data,name);
    O->FParentTools		= this;
    return O;
}

