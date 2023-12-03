#pragma once

class ESceneWayTool: public FRBMKSceneObjectsToolBase
{
	typedef FRBMKSceneObjectsToolBase inherited;
public:
					ESceneWayTool			():FRBMKSceneObjectsToolBase(ERBMKSceneObjectType::Way){;}
                    ~ESceneWayTool          () override;
	// definition
    LPCSTR			ClassName				() override {return "way";}

    void		    Clear					() override  {inherited::Clear();}
    // IO
    bool   		    LoadStream            	(IReader&) override;
    bool   		    LoadLTX            		(CInifile&) override;
    bool		    LoadSelection      		(IReader&) override;

    TSharedPtr<FXRayCustomObject> CreateObject			    (LPVOID data, LPCSTR name) override;
};