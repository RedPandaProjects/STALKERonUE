#pragma once

class ESceneShapeTool: public ESceneCustomOTool
{
	typedef ESceneCustomOTool inherited;
public:
						ESceneShapeTool			():ESceneCustomOTool(OBJCLASS_SHAPE){;}
	// definition
    IC LPCSTR			ClassName				(){return "shape";}
    IC LPCSTR			ClassDesc				(){return "Shape";}

    virtual void		Clear					(){inherited::Clear();}
    // IO
    virtual bool   		LoadStream            		(IReader&);
    virtual bool   		LoadLTX            		(CInifile&);
    virtual bool		LoadSelection      		(IReader&);

    virtual CCustomObject* CreateObject			(LPVOID data, LPCSTR name);
};