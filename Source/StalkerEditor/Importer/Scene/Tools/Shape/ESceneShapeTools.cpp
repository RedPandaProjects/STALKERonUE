#include "ESceneShapeTools.h"
#include "../../Entitys/ShapeObject/EShape.h"


CCustomObject* ESceneShapeTool::CreateObject(LPVOID data, LPCSTR name)
{
	CCustomObject* O	= new  CEditShape(data,name);
    O->FParentTools		= this;
    return O;
}

