#pragma once

class FRBMKSceneParticleSystemsTool: public FRBMKSceneObjectsToolBase
{
public:
					                FRBMKSceneParticleSystemsTool	();
                                    ~FRBMKSceneParticleSystemsTool  () override;

    const TCHAR*	                ClassName				        () override {return TEXT("ps");}

    bool   		                    LoadStream            	        (IReader&) override;
    bool   		                    LoadLTX            		        (CInifile&) override;
    bool		                    LoadSelection      		        (IReader&) override;
    
	TSharedPtr<FRBMKSceneObjectBase>CreateObject			        (const FString&Name) override;

    void                            ExportToWorld                   (UWorld*World,EObjectFlags InFlags,const URBMKLevelImportOptions&LevelImportOptions) override;

private:
	static constexpr  uint16 ToolVersion  	= 0x0000;
	static constexpr  uint32 ChunkVersion   = 0x1001ul;
};