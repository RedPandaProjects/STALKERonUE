#include "ESceneCustomMTools.h"

#define CHUNK_TOOLS_TAG	0x7777

ESceneToolBase::ESceneToolBase(ObjClassID cls)
{
    FClassID				= cls;
}

ESceneToolBase::~ESceneToolBase()
{
}
bool ESceneToolBase::LoadLTX(CInifile& ini)
{
   
	return 			true;
}

bool ESceneToolBase::LoadStream(IReader& F)
{
	
    return true;
}

void ESceneToolBase::Clear()
{
}