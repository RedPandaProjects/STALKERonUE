#pragma once

class FRBMKSceneShapesTool: public FRBMKSceneObjectsToolBase
{
public:
						                FRBMKSceneShapesTool    ();
						                ~FRBMKSceneShapesTool   ();

    const TCHAR*		                ClassName		        () override { return TEXT("shape");}

    bool   		                        LoadStream              (IReader&) override;
    bool   		                        LoadLTX                 (CInifile&) override;
    bool		                        LoadSelection           (IReader&) override;

    TSharedPtr<FRBMKSceneObjectBase>    CreateObject            (const FString&Name) override;
private:
    static constexpr uint32             ToolVersion             = 0x0000;
	static constexpr uint32             ChunkVersion            = 0x1001ul;
};