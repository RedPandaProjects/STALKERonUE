#include "ESceneObjectTools.h"
#include "../../Entitys/StaticObject/SceneObject.h"

ESceneObjectTool::ESceneObjectTool():ESceneCustomOTool(OBJCLASS_SCENEOBJECT)
{
}


ESceneObjectTool::~ESceneObjectTool()
{

}

void ESceneObjectTool::Clear		()
{
	inherited::Clear				();
}


CCustomObject* ESceneObjectTool::CreateObject(LPVOID data, LPCSTR name)
{
	CCustomObject* O	= new CSceneObject(data,name);                     
    O->FParentTools		= this;
    return O;
}

