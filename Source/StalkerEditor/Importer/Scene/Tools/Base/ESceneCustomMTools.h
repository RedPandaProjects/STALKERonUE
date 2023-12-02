#pragma once

class FXRaySceneToolBase
{
public:
											FXRaySceneToolBase      (EXRayObjectClassID cls);
    virtual                                 ~FXRaySceneToolBase     ();

	bool                                    IsEnabled               () { return true; }
	bool		                            IsEditable              () { return true; }
    virtual bool                            LoadStream            	(IReader&)  = 0;
    virtual bool                            LoadLTX            		(CInifile&) = 0;
    virtual bool                            CanUseInifile			() {return true;}
    virtual void                            Clear                   () = 0;
	virtual const char*                     ClassName               () = 0;

    virtual bool		                    LoadSelection      		(IReader&)  = 0;
    virtual class FXRaySceneCustomOTool*    CastToSceneCustomOTool  () {return nullptr;}
    virtual void                            ExportToCurrentWorld    (){};
    
    EXRayObjectClassID                              FClassID;
};
