#pragma once

class EScenePSTools: public FXRaySceneCustomOTool
{
	typedef FXRaySceneCustomOTool inherited;
public:
					EScenePSTools			():FXRaySceneCustomOTool(OBJCLASS_PS){;}
                    ~EScenePSTools          () override;
	// definition
    LPCSTR			ClassName				() override {return "ps";}

    void		    Clear					() override  {inherited::Clear();}
    // IO
    bool   		    LoadStream            	(IReader&) override;
    bool   		    LoadLTX            		(CInifile&) override;
    bool		    LoadSelection      		(IReader&) override;

    FXRayCustomObject* CreateObject			    (LPVOID data, LPCSTR name) override;
};