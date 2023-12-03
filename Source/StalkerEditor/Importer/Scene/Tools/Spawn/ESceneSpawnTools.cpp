#include "ESceneSpawnTools.h"
#include "../../Entitys/SpawnObject/SpawnPoint.h"


ESceneSpawnTool::ESceneSpawnTool():FRBMKSceneObjectsToolBase(ERBMKSceneObjectType::SpawnPoint)
{
	m_Flags.zero();
}

ESceneSpawnTool::~ESceneSpawnTool()
{

}

TSharedPtr<FXRayCustomObject> ESceneSpawnTool::CreateObject(LPVOID data, LPCSTR name)
{
	return MakeShared<CSpawnPoint>(data,name);
}

