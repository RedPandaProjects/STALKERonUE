#pragma once

class EScenePSTools: public FRBMKSceneObjectsToolBase
{
	typedef FRBMKSceneObjectsToolBase inherited;
public:
					EScenePSTools			():FRBMKSceneObjectsToolBase(ERBMKSceneObjectType::ParticleSystem){;}
                    ~EScenePSTools          () override;
	// definition
    LPCSTR			ClassName				() override {return "ps";}

    void		    Clear					() override  {inherited::Clear();}
    // IO
    bool   		    LoadStream            	(IReader&) override;
    bool   		    LoadLTX            		(CInifile&) override;
    bool		    LoadSelection      		(IReader&) override;

  TSharedPtr<FXRayCustomObject> CreateObject			    (LPVOID data, LPCSTR name) override;
};