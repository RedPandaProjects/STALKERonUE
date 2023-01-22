#include "ESceneWayTools.h"

// chunks
static const u16 WAY_TOOLS_VERSION  	= 0x0000;

enum{
    CHUNK_VERSION			= 0x1001ul,
};

bool ESceneWayTool::LoadLTX(CInifile& ini)
{
	u32 version 	= ini.r_u32("main","version");
    if( version!=WAY_TOOLS_VERSION )
    {
            return false;
    }

	inherited::LoadLTX(ini);
	return true;
}

bool ESceneWayTool::LoadStream(IReader& F)
{
	u16 version 	= 0;
    if(F.r_chunk(CHUNK_VERSION,&version))
        if( version!=WAY_TOOLS_VERSION )
        {
            return false;
        }

	if (!inherited::LoadStream(F)) return false;

    return true;
}




bool ESceneWayTool::LoadSelection(IReader& F)
{
	u16 version 	= 0;
    check(F.r_chunk(CHUNK_VERSION,&version));
    if( version!=WAY_TOOLS_VERSION )
    {
        return false;
    }

	return inherited::LoadSelection(F);
}




