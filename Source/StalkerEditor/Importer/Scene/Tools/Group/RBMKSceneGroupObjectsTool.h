#pragma once

class FRBMKSceneGroupObjectsTool: public FRBMKSceneObjectsToolBase
{
 public:
				                                FRBMKSceneGroupObjectsTool		    ():FRBMKSceneObjectsToolBase(ERBMKSceneObjectType::Group){;}
    const TCHAR*		                        ClassName			    () override {return TEXT("group");}
    bool   		                                LoadStream              (IReader&) override;
    bool   		                                LoadLTX            	    (CInifile&) override;
    bool		                                LoadSelection      	    (IReader&) override;
    TSharedPtr<FRBMKSceneObjectBase>            CreateObject			(const FString&Name) override;
private:
	static constexpr  uint16 ToolVersion  	= 0x0000;
	static constexpr  uint32 ChunkVersion   = 0x1001ul;
};