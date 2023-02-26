#include "ESceneGroupTools.h"
#include "../../Entitys/GroupObject/GroupObject.h"
constexpr u16 GROUP_TOOLS_VERSION  	= 0x0000;
#define  CHUNK_VERSION			 0x1001ul
CCustomObject* ESceneGroupTool::CreateObject(LPVOID data, LPCSTR name)
{
	CCustomObject* O	= xr_new<CGroupObject>(data, name);
    O->FParentTools		= this;
    return O;
}

bool ESceneGroupTool::LoadStream(IReader& F)
{
	u16 version 	= 0;
    if(F.r_chunk(CHUNK_VERSION,&version))
        if( version!=GROUP_TOOLS_VERSION )
        {
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
        }

	if (!inherited::LoadStream(F)) return false;

    return true;
}

bool ESceneGroupTool::LoadLTX(CInifile& ini)
{
	LPCSTR section = "main";
	u16 version 	= ini.r_u16(section, "version");

    if( version!=GROUP_TOOLS_VERSION )
    {
        ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
        return false;
    }
	if (!inherited::LoadLTX(ini)) return false;

    return true;
}

bool ESceneGroupTool::LoadSelection(IReader& F)
{
	u16 version 	= 0;
    R_ASSERT(F.r_chunk(CHUNK_VERSION,&version));
    if( version!=GROUP_TOOLS_VERSION )
    {
        ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
        return false;
    }

	return inherited::LoadSelection(F);
}