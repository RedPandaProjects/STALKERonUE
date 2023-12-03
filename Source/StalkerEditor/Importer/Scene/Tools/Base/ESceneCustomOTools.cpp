#include "ESceneCustomOTools.h"

FRBMKSceneObjectsToolBase::FRBMKSceneObjectsToolBase(ERBMKSceneObjectType cls):FRBMKSceneToolBase(cls)
{
}


FRBMKSceneObjectsToolBase::~FRBMKSceneObjectsToolBase()
{
}

TSharedPtr<FXRayCustomObject>  FRBMKSceneObjectsToolBase::FindObjectByName(LPCSTR name)
{
	for(TSharedPtr<FXRayCustomObject>& Object :Objects)
	{
		LPCSTR _name = Object->GetName();
		if (0 == FCStringAnsi::Strcmp(_name, name))
		{
			return Object;
		}
	}
	return nullptr;
}

