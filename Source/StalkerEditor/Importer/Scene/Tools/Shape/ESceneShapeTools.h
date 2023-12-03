#pragma once

class ESceneShapeTool: public FRBMKSceneObjectsToolBase
{
	typedef FRBMKSceneObjectsToolBase inherited;
public:
						ESceneShapeTool			():FRBMKSceneObjectsToolBase(ERBMKSceneObjectType::Shape){;}
	// definition
    IC LPCSTR			ClassName				(){return "shape";}
    IC LPCSTR			ClassDesc				(){return "Shape";}

    virtual void		Clear					(){inherited::Clear();}
    // IO
    virtual bool   		LoadStream            		(IReader&);
    virtual bool   		LoadLTX            		(CInifile&);
    virtual bool		LoadSelection      		(IReader&);

    virtual TSharedPtr<FXRayCustomObject> CreateObject			(LPVOID data, LPCSTR name);
};