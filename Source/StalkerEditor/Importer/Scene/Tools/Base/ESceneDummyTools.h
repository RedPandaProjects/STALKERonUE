#pragma once

class ESceneDummyTool: public FXRaySceneCustomOTool
{
	typedef FXRaySceneCustomOTool inherited;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						ESceneDummyTool			():FXRaySceneCustomOTool(ERBMKSceneObjectType::AllTypes){;}
                        
    virtual void		Clear					(){inherited::Clear();}
	// definition
    IC LPCSTR			ClassName				(){THROW; return 0;}
    IC LPCSTR			ClassDesc				(){THROW; return 0;}
    IC int				RenderPriority			(){return -1; }

    virtual bool   		IsNeedSave				(){return false;}

    virtual FXRayCustomObject* CreateObject			(LPVOID data, LPCSTR name){return 0;};
};
