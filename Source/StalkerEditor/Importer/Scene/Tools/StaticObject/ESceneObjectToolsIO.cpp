#include "ESceneObjectTools.h"

static const u16 OBJECT_TOOLS_VERSION  	= 0x0000;

 static u32   OBJ_CHUNK_VERSION			= 0x1001ul;
bool ESceneObjectTool::LoadLTX(CInifile& ini)
{
	u32 version 	= ini.r_u32("main", "version");
    if( version!=OBJECT_TOOLS_VERSION )
    {
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassName());
            return false;
    }

	inherited::LoadLTX		(ini);


    return true;
}

bool ESceneObjectTool::LoadStream(IReader& F)
{
	u16 version 	= 0;
    if(F.r_chunk(OBJ_CHUNK_VERSION,&version)){
        if( version!=OBJECT_TOOLS_VERSION ){
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.", ClassName());
            return false;
        }
    }
	if (!inherited::LoadStream(F)) return false;


    return true;
}


bool ESceneObjectTool::LoadSelection(IReader& F)
{
	u16 version 	= 0;
    R_ASSERT(F.r_chunk(OBJ_CHUNK_VERSION,&version));
    if( version!=OBJECT_TOOLS_VERSION ){
        ELog.DlgMsg( mtError, "%s tools: Unsupported version.", ClassName());
        return false;
    }

	return inherited::LoadSelection(F);
}



