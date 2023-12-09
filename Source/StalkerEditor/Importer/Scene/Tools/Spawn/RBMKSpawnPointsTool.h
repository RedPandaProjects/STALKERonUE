#pragma once

class FRBMKSpawnPointsTool: public FRBMKSceneObjectsToolBase
{
public:
						                FRBMKSpawnPointsTool	();
						                ~FRBMKSpawnPointsTool   () override;

    const TCHAR*	                    ClassName		        () override {return TEXT("spawn");}

    bool   		                        LoadStream              (IReader&) override;
    bool   		                        LoadLTX                 (CInifile&) override;
    bool		                        LoadSelection           (IReader&) override;

    TSharedPtr<FRBMKSceneObjectBase>    CreateObject	        (const FString&Name) override;

    void                                ExportToWorld           (UWorld*World,EObjectFlags InFlags,const UXRayLevelImportOptions&LevelImportOptions) override;
private:
    static constexpr uint32             ToolVersion             = 0x0000;
	static constexpr uint32             ChunkVersion            = 0x1001ul;
};