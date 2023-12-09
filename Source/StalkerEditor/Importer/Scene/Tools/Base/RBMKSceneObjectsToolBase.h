#pragma once
#include "RBMKSceneToolBase.h"
class FRBMKSceneObjectsToolBase: public FRBMKSceneToolBase
{
public:
												FRBMKSceneObjectsToolBase	(ERBMKSceneObjectType InObjectType);
												~FRBMKSceneObjectsToolBase	();

    bool   										LoadStream            		(IReader&);
    bool   										LoadLTX            			(CInifile&);
    bool										LoadSelection      			(IReader&);
	
	TSharedPtr<FRBMKSceneObjectBase>			FindObjectByName			(const FString&Name);

    virtual TSharedPtr<FRBMKSceneObjectBase>	CreateObject				(const FString&Name) = 0;
	FRBMKSceneObjectsToolBase*					CastToSceneCustomOTool		() override { return this; }
	TArray<TSharedPtr<FRBMKSceneObjectBase>>	Objects;
};