#include "ESceneShapeTools.h"
#include "../../Entitys/ShapeObject/EShape.h"


FXRayCustomObject* ESceneShapeTool::CreateObject(LPVOID data, LPCSTR name)
{
	FXRayCustomObject* O	= new  CEditShape(data,name);
    O->FParentTools		= this;
    return O;
}

