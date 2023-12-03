#include "ESceneObjectTools.h"
#include "../../Entitys/StaticObject/SceneObject.h"

ESceneObjectTool::ESceneObjectTool():FXRaySceneCustomOTool(ERBMKSceneObjectType::SceneObject)
{
}


ESceneObjectTool::~ESceneObjectTool()
{

}

void ESceneObjectTool::Clear		()
{
	inherited::Clear				();
}


TSharedPtr<FXRayCustomObject> ESceneObjectTool::CreateObject(LPVOID data, LPCSTR name)
{
	return MakeShared<CSceneObject>(data,name);
}

