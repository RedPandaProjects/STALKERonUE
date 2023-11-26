#pragma once

class EScenePSTools: public ESceneCustomOTool
{
	typedef ESceneCustomOTool inherited;
public:
					EScenePSTools			():ESceneCustomOTool(OBJCLASS_PS){;}
                    ~EScenePSTools          () override;
	// definition
    LPCSTR			ClassName				() override {return "ps";}

    void		    Clear					() override  {inherited::Clear();}
    // IO
    bool   		    LoadStream            	(IReader&) override;
    bool   		    LoadLTX            		(CInifile&) override;
    bool		    LoadSelection      		(IReader&) override;

    CCustomObject* CreateObject			    (LPVOID data, LPCSTR name) override;
};