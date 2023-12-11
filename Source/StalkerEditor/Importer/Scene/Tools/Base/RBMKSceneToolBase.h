#pragma once
#include "Importer/ImporterFactory/RBMKLevelImportOptions.h"

class FRBMKSceneToolBase
{
public:
											FRBMKSceneToolBase      (ERBMKSceneObjectType InObjectType);
    virtual                                 ~FRBMKSceneToolBase     ();

    virtual bool                            LoadStream            	(IReader&)  = 0;
    virtual bool		                    LoadSelection      		(IReader&)  = 0;
    virtual bool                            LoadLTX            		(CInifile&) = 0;
    virtual bool                            CanUseInifile			() {return true;}
	virtual const TCHAR*                    ClassName               () = 0;

    virtual void                            ExportToWorld           (UWorld*World,EObjectFlags InFlags,const URBMKLevelImportOptions&LevelImportOptions) {};
    virtual class FRBMKSceneObjectsToolBase*CastToSceneCustomOTool  () {return nullptr;}
    
    ERBMKSceneObjectType                    ObjectType;
};
