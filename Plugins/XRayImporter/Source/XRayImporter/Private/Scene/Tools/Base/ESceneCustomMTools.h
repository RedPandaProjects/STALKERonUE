#pragma once
class ESceneCustomMTools;

class ESceneToolBase
{
public:
                                         ESceneToolBase          (ObjClassID cls);
    virtual                             ~ESceneToolBase          ();

    ObjClassID 			                FClassID;
	BOOL				                IsEnabled               () { return TRUE; }
	BOOL				                IsEditable              () { return TRUE; }
    virtual bool		                LoadStream            	(IReader&)=0;
    virtual bool   		                LoadLTX            		(CInifile&)=0;
    virtual bool		                can_use_inifile			() {return true;}
    virtual void                        Clear                   () = 0;
	virtual LPCSTR		                ClassName               () = 0;

    virtual bool		                LoadSelection      		(IReader&)=0;
    virtual class ESceneCustomOTool*    CastToESceneCustomOTool () {return nullptr;}
};

DEFINE_MAP(ObjClassID,ESceneToolBase*,SceneToolsMap,SceneToolsMapPairIt);