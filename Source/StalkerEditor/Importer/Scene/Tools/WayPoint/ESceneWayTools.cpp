#include "ESceneWayTools.h"
#include "../../Entitys/WayObject/WayPoint.h"

ESceneWayTool::~ESceneWayTool()
{

}

TSharedPtr<FXRayCustomObject> ESceneWayTool::CreateObject(LPVOID data, LPCSTR name)
{
    return MakeShared<CWayObject>(data,name);
}


