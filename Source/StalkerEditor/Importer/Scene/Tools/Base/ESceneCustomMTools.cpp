#include "ESceneCustomMTools.h"

#define CHUNK_TOOLS_TAG	0x7777

FXRaySceneToolBase::FXRaySceneToolBase(EXRayObjectClassID cls)
{
    FClassID				= cls;
}

FXRaySceneToolBase::~FXRaySceneToolBase()
{
}

bool FXRaySceneToolBase::LoadLTX(CInifile& ini)
{
	return 			true;
}

bool FXRaySceneToolBase::LoadStream(IReader& F)
{
    return true;
}

void FXRaySceneToolBase::Clear()
{
}