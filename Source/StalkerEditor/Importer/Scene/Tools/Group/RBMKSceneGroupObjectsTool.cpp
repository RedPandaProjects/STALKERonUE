#include "RBMKSceneGroupObjectsTool.h"

#include "Importer/Scene/Entitys/GroupObject/RBMKSceneGroupObject.h"

bool FRBMKSceneGroupObjectsTool::LoadStream(IReader& F)
{
	uint16 Version 	= 0;
    if(F.r_chunk(ChunkVersion,&Version))
    {
    	if(Version != ToolVersion )
        {
			UE_LOG(LogXRayImporter,Error,TEXT("%s tools: Unsupported version."),ClassName());
            return false;
        }
    }
    return FRBMKSceneObjectsToolBase::LoadStream(F);
}

bool FRBMKSceneGroupObjectsTool::LoadLTX(CInifile& ini)
{
	const uint32 Version 	= ini.r_u32("main","version");
    if(Version != ToolVersion )
    {
		UE_LOG(LogXRayImporter,Error,TEXT("%s tools: Unsupported version."),ClassName());
		return false;
    }
	return FRBMKSceneObjectsToolBase::LoadLTX(ini);
}

bool FRBMKSceneGroupObjectsTool::LoadSelection(IReader& F)
{
	uint16 Version 	= 0;
    if(F.r_chunk(ChunkVersion,&Version))
    {
    	if(Version != ToolVersion )
        {
			UE_LOG(LogXRayImporter,Error,TEXT("%s tools: Unsupported version."),ClassName());
            return false;
        }
    }
    return FRBMKSceneObjectsToolBase::LoadSelection(F);
}

TSharedPtr<FRBMKSceneObjectBase> FRBMKSceneGroupObjectsTool::CreateObject(const FString& Name)
{
    return MakeShared<FRBMKSceneGroupObject>(Name); 
}
