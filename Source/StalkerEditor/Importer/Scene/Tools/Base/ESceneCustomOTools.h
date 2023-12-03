#pragma once
#include "RBMKSceneToolBase.h"
class FRBMKSceneObjectsToolBase: public FRBMKSceneToolBase
{
public:
											FRBMKSceneObjectsToolBase	(ERBMKSceneObjectType cls);
											~FRBMKSceneObjectsToolBase	();

    bool   									LoadStream            		(IReader&);
    bool   									LoadLTX            			(CInifile&);
    bool									LoadSelection      			(IReader&);
	
	TSharedPtr<FXRayCustomObject>			FindObjectByName			(LPCSTR name);

    virtual TSharedPtr<FXRayCustomObject>	CreateObject				(LPVOID data, LPCSTR name) = 0;
	FRBMKSceneObjectsToolBase*				CastToSceneCustomOTool		() override { return this; }
	TArray<TSharedPtr<FXRayCustomObject>>	Objects;
};