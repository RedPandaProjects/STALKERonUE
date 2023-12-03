#include "ESceneSpawnTools.h"
#include "../../Entitys/SpawnObject/SpawnPoint.h"


ESceneSpawnTool::ESceneSpawnTool():FXRaySceneCustomOTool(ERBMKSceneObjectType::SpawnPoint)
{
	m_Flags.zero();
}

ESceneSpawnTool::~ESceneSpawnTool()
{

}

FXRayCustomObject* ESceneSpawnTool::CreateObject(LPVOID data, LPCSTR name)
{
	CSpawnPoint* O	= new CSpawnPoint(data,name);
    O->FParentTools		= this;
    return O;
}

