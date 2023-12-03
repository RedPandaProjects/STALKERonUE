#include "RBMKSceneToolBase.h"

FRBMKSceneToolBase::FRBMKSceneToolBase(ERBMKSceneObjectType InObjectType):ObjectType(InObjectType)
{
}

FRBMKSceneToolBase::~FRBMKSceneToolBase()
{
}

bool FRBMKSceneToolBase::LoadLTX(CInifile& ini)
{
	return 			true;
}

bool FRBMKSceneToolBase::LoadStream(IReader& F)
{
    return true;
}

void FRBMKSceneToolBase::Clear()
{
}