#include "FRBMKSceneWallmarkTool.h"

#include "StalkerEditorManager.h"
#include "Importer/FRBMKEngineFactory.h"

FRBMKSceneWallmarkTool::FRBMKSceneWallmarkTool() :FRBMKSceneToolBase(ERBMKSceneObjectType::Wallmark)
{

}

FRBMKSceneWallmarkTool::~FRBMKSceneWallmarkTool()
{
}


bool FRBMKSceneWallmarkTool::LoadStream(IReader&F)
{
	enum
	{
		WM_VERSION  	 = 0x0004,
		WM_CHUNK_VERSION = 0x0001,     
		WM_CHUNK_FLAGS	 = 0x0002,
		WM_CHUNK_PARAMS	 = 0x0003,
		WM_CHUNK_ITEMS	 = 0x0004,
		WM_CHUNK_ITEMS2	 = 0x0005,
	};


    FRBMKSceneToolBase::LoadStream	(F);

	u16 version = 0;

    if(!ensure(F.r_chunk(WM_CHUNK_VERSION,&version)))
    {
	    return false;
    }


    if(version!=0x0003 && version!=WM_VERSION)
    {
    	UE_LOG(LogXRayImporter,Error,TEXT("%s tools: Unsupported version."),ClassName());
        return false;
    }


    if ( IReader* ChunkItems = F.open_chunk(WM_CHUNK_ITEMS2))
    {
        IReader* ChunkItem = ChunkItems->open_chunk(0);
        for (int32 Count=1; ChunkItem; Count++)
        {
            const int32 ItemCount	= static_cast<int32>(ChunkItem->r_u32());
            if (ItemCount)
            {
                shared_str		InTextureName,InShaderName;
                ChunkItem->r_stringZ	(InShaderName);
                ChunkItem->r_stringZ	(InTextureName);
                for(int32 i = 0; i<ItemCount; i++)
                {
	                FRBMKSceneWallmarkItem&Item = Items.AddDefaulted_GetRef();
                    Item.TextureName = ANSI_TO_TCHAR(*InTextureName);
                    Item.ShaderName = ANSI_TO_TCHAR(*InShaderName);
                    ChunkItem->advance(1);

                    Fbox InBox;
                    ChunkItem->r(&InBox,sizeof(Fbox));
                    Fvector InPosition;
                    InBox.getcenter(InPosition);
                    Item.Position = StalkerMath::RBMKLocationToUnreal(InPosition);

                    ChunkItem->advance(16);
					Item.Width  = ChunkItem->r_float();
					Item.Hieght = ChunkItem->r_float();
					Item.Rotate = ChunkItem->r_float();
                    uint32 VertexCount = ChunkItem->r_u32();
                    if(ensure(VertexCount>=3))
                    {
                        FVector3f Vertices[3];
                        Fvector InVertexPosition;
                        for(int32 j=0;j<3;j++)
                        {
							ChunkItem->r(&InVertexPosition,sizeof(Fvector));
							ChunkItem->advance(12);
	                        Vertices[j] = StalkerMath::RBMKLocationToUnreal(InVertexPosition);  
                        }
	                    VertexCount-=3;
                        Item.Plane = FPlane4f(Vertices[0],Vertices[1],Vertices[2]);
                    }
                    ChunkItem->advance(static_cast<int32>(24*VertexCount));
                }
            }
            ChunkItem->close();
            ChunkItem = ChunkItems->open_chunk(Count);
        }
        ChunkItems->close();
    }
    return true;
}

bool FRBMKSceneWallmarkTool::LoadLTX(CInifile&)
{
	return false;
}

bool FRBMKSceneWallmarkTool::LoadSelection(IReader&F)
{
	return LoadStream(F);
}


void FRBMKSceneWallmarkTool::ExportToWorld(UWorld* World, EObjectFlags InFlags,const URBMKLevelImportOptions&LevelImportOptions)
{
	FRBMKEngineFactory EngineFactory(nullptr,InFlags);
	for( FRBMKSceneWallmarkItem&Item:Items)
	{
		FQuat Rotation = FQuat(Item.Plane.ToOrientationQuat())*FRotator(-90,0,0).Quaternion();
		Rotation = Rotation*FRotator(0,FMath::RadiansToDegrees(Item.Rotate)-90,0).Quaternion();
		ADecalActor* DecalActor = World->SpawnActor<ADecalActor>(FVector(Item.Position), Rotation.Rotator());
		DecalActor->SetFolderPath(TEXT("Decals"));
		DecalActor->GetDecal()->DecalSize = FVector(FMath::Max(Item.Width,Item.Hieght)*12.5f,Item.Hieght*50,Item.Width*50);
		DecalActor->SetDecalMaterial(EngineFactory.ImportSurface(TEXT(""), TCHAR_TO_ANSI(*Item.ShaderName), TCHAR_TO_ANSI(*Item.TextureName), ""));
	}
}
