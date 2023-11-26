#include "EScenePSTools.h"

// chunks
static const u16 PS_TOOLS_VERSION  	= 0x0000;

#define  CHUNK_VERSION			 0x1001ul
bool EScenePSTools::LoadLTX(CInifile& ini)
{
	u32 version 	= ini.r_u32("main","version");
    if( version!=PS_TOOLS_VERSION )
    {
            return false;
    }

	inherited::LoadLTX(ini);
	return true;
}

bool EScenePSTools::LoadStream(IReader& F)
{
	u16 version 	= 0;
    if(F.r_chunk(CHUNK_VERSION,&version))
        if( version!=PS_TOOLS_VERSION )
        {
            return false;
        }

	if (!inherited::LoadStream(F)) return false;

    return true;
}




bool EScenePSTools::LoadSelection(IReader& F)
{
	u16 version 	= 0;
    check(F.r_chunk(CHUNK_VERSION,&version));
    if( version!=PS_TOOLS_VERSION )
    {
        return false;
    }

	return inherited::LoadSelection(F);
}




