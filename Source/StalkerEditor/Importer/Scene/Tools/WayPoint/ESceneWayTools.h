#pragma once

class ESceneWayTool: public ESceneCustomOTool
{
	typedef ESceneCustomOTool inherited;
public:
					ESceneWayTool			():ESceneCustomOTool(OBJCLASS_WAY){;}
                    ~ESceneWayTool          () override;
	// definition
    LPCSTR			ClassName				() override {return "way";}

    void		    Clear					() override  {inherited::Clear();}
    // IO
    bool   		    LoadStream            	(IReader&) override;
    bool   		    LoadLTX            		(CInifile&) override;
    bool		    LoadSelection      		(IReader&) override;

    CCustomObject* CreateObject			    (LPVOID data, LPCSTR name) override;
};