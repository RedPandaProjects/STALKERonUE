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


FXRayCustomObject* ESceneObjectTool::CreateObject(LPVOID data, LPCSTR name)
{
	FXRayCustomObject* O	= new CSceneObject(data,name);                     
    O->FParentTools		= this;
    return O;
}

