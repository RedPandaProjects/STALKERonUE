#pragma once

class FRBMKSceneToolBase
{
public:
											FRBMKSceneToolBase      (ERBMKSceneObjectType InObjectType);
    virtual                                 ~FRBMKSceneToolBase     ();

    virtual bool                            LoadStream            	(IReader&)  = 0;
    virtual bool		                    LoadSelection      		(IReader&)  = 0;
    virtual bool                            LoadLTX            		(CInifile&) = 0;
    virtual bool                            CanUseInifile			() {return true;}
    virtual void                            Clear                   () = 0;
	virtual const char*                     ClassName               () = 0;

    virtual void                            ExportToCurrentWorld    ()  {};
    virtual class FRBMKSceneObjectsToolBase*    CastToSceneCustomOTool  () {return nullptr;}
    
    ERBMKSceneObjectType                    ObjectType;
};
