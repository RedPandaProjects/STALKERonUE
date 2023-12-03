#pragma once

class ESceneObjectTool: public FRBMKSceneObjectsToolBase
{
	typedef FRBMKSceneObjectsToolBase inherited;
public:
    ESceneObjectTool();
    ~ESceneObjectTool() override;
	// definition
    LPCSTR			ClassName				() override{return "scene_object";}


    void		Clear					() override;
    // IO

    bool   		LoadStream            		(IReader&) override;
    bool   		LoadLTX            		(CInifile&) override;
    bool		LoadSelection      		(IReader&) override;

   TSharedPtr<FXRayCustomObject> CreateObject			(LPVOID data, LPCSTR name) override;
};