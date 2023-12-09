#include "RBMKSceneShapesTool.h"

#include "Importer/Scene/Entitys/Shape/RBMKSceneShape.h"

FRBMKSceneShapesTool::FRBMKSceneShapesTool():FRBMKSceneObjectsToolBase(ERBMKSceneObjectType::Shape)
{
}

FRBMKSceneShapesTool::~FRBMKSceneShapesTool()
{
}

bool FRBMKSceneShapesTool::LoadLTX(CInifile& ini)
{
	const uint32 Version 	= ini.r_u32("main","version");
    if(Version != ToolVersion )
    {
		UE_LOG(LogXRayImporter,Error,TEXT("%s tools: Unsupported version."),ClassName());
		return false;
    }
	FRBMKSceneObjectsToolBase::LoadLTX(ini);
	return true;
}

bool FRBMKSceneShapesTool::LoadStream(IReader& F)
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

bool FRBMKSceneShapesTool::LoadSelection(IReader& F)
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

TSharedPtr<FRBMKSceneObjectBase> FRBMKSceneShapesTool::CreateObject(const FString&Name)
{
    return MakeShared<FRBMKSceneShape>(Name);
}

