#include "ESceneShapeTools.h"

// chunks
static const u16 SHAPE_TOOLS_VERSION  	= 0x0000;

enum{
    CHUNK_VERSION			= 0x1001ul,
};

bool ESceneShapeTool::LoadLTX(CInifile& ini)
{
	u32 version 	= ini.r_u32("main","version");
    if( version!=SHAPE_TOOLS_VERSION )
    {
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
    }

	inherited::LoadLTX(ini);
	return true;
}

bool ESceneShapeTool::LoadStream(IReader& F)
{
	u16 version 	= 0;
    if(F.r_chunk(CHUNK_VERSION,&version))
        if( version!=SHAPE_TOOLS_VERSION ){
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
        }

	if (!inherited::LoadStream(F)) return false;

    return true;
}



bool ESceneShapeTool::LoadSelection(IReader& F)
{
	u16 version 	= 0;
    R_ASSERT(F.r_chunk(CHUNK_VERSION,&version));
    if( version!=SHAPE_TOOLS_VERSION ){
        ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
        return false;
    }

	return inherited::LoadSelection(F);
}

