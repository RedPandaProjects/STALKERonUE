#pragma once

class ESceneShapeTool: public FXRaySceneCustomOTool
{
	typedef FXRaySceneCustomOTool inherited;
public:
						ESceneShapeTool			():FXRaySceneCustomOTool(OBJCLASS_SHAPE){;}
	// definition
    IC LPCSTR			ClassName				(){return "shape";}
    IC LPCSTR			ClassDesc				(){return "Shape";}

    virtual void		Clear					(){inherited::Clear();}
    // IO
    virtual bool   		LoadStream            		(IReader&);
    virtual bool   		LoadLTX            		(CInifile&);
    virtual bool		LoadSelection      		(IReader&);

    virtual FXRayCustomObject* CreateObject			(LPVOID data, LPCSTR name);
};