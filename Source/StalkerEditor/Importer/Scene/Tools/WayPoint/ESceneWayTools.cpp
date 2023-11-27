#include "ESceneWayTools.h"
#include "../../Entitys/WayObject/WayPoint.h"

ESceneWayTool::~ESceneWayTool()
{

}

FXRayCustomObject* ESceneWayTool::CreateObject(LPVOID data, LPCSTR name)
{
	FXRayCustomObject* O	= new CWayObject(data,name);
    O->FParentTools		= this;
    return O;
}


