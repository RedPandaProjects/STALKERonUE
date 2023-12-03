#include "ESceneShapeTools.h"
#include "../../Entitys/ShapeObject/EShape.h"


TSharedPtr<FXRayCustomObject> ESceneShapeTool::CreateObject(LPVOID data, LPCSTR name)
{
    return MakeShared<CEditShape>(data,name);
}

