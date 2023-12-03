#pragma once

class ESceneWayTool: public FXRaySceneCustomOTool
{
	typedef FXRaySceneCustomOTool inherited;
public:
					ESceneWayTool			():FXRaySceneCustomOTool(ERBMKSceneObjectType::Way){;}
                    ~ESceneWayTool          () override;
	// definition
    LPCSTR			ClassName				() override {return "way";}

    void		    Clear					() override  {inherited::Clear();}
    // IO
    bool   		    LoadStream            	(IReader&) override;
    bool   		    LoadLTX            		(CInifile&) override;
    bool		    LoadSelection      		(IReader&) override;

    FXRayCustomObject* CreateObject			    (LPVOID data, LPCSTR name) override;
};