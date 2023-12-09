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
                    Item.Position = StalkerMath::XRayLocationToUnreal(InPosition);

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
	                        Vertices[j] = StalkerMath::XRayLocationToUnreal(InVertexPosition);  
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


void FRBMKSceneWallmarkTool::ExportToWorld(UWorld* World, EObjectFlags InFlags,const UXRayLevelImportOptions&LevelImportOptions)
{
	for( FRBMKSceneWallmarkItem&Item:Items)
	{
		FQuat Rotation = FQuat(Item.Plane.ToOrientationQuat())*FRotator(-90,0,0).Quaternion();
		Rotation = Rotation*FRotator(0,FMath::RadiansToDegrees(Item.Rotate)-90,0).Quaternion();
		ADecalActor* DecalActor = World->SpawnActor<ADecalActor>(FVector(Item.Position), Rotation.Rotator());
		DecalActor->SetFolderPath(TEXT("Decals"));
		DecalActor->GetDecal()->DecalSize = FVector(FMath::Max(Item.Width,Item.Hieght)*12.5f,Item.Hieght*50,Item.Width*50);
		DecalActor->SetDecalMaterial(ImportSurfaceForDecal(Item.ShaderName,Item.TextureName,InFlags));
	}
}

UMaterialInterface* FRBMKSceneWallmarkTool::ImportSurfaceForDecal(const FString& ShaderName, const FString& TextureName,EObjectFlags InFlags)
{
    if (ShaderName.IsEmpty())
		return nullptr;
	FString ParentName = ShaderName.Replace(TEXT("\\"), TEXT("/"));
	FString GlobalMaterialInstanceName =  UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("MaterialsInstance") / ParentName/( FPaths::ChangeExtension(TextureName, TEXT("")).Replace(TEXT("\\"), TEXT("/"))));

	UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, *(GlobalMaterialInstanceName + TEXT(".") + FPaths::GetBaseFilename(GlobalMaterialInstanceName)), nullptr, LOAD_NoWarn);
	if (Material)
	{
		return Material;
	}

	UMaterialInterface* ParentMaterial = nullptr;
	{
		const FString ParentPackageName = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Materials") / ParentName);
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	if (!IsValid(ParentMaterial))
	{
		const FString ParentPackageName = UPackageTools::SanitizePackageName(TEXT("/Game/Base/Materials") / ParentName);
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	if (!IsValid(ParentMaterial))
	{
		UMaterialInterface* UnkownMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Base/Materials/Decals/Unkown.Unkown"));
		check(IsValid(UnkownMaterial));
		const FString ParentPackageName = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Materials") / ParentName);
		UPackage* AssetPackage = CreatePackage(*ParentPackageName);
		UMaterialInstanceConstant* NewParentMaterial = NewObject<UMaterialInstanceConstant>(AssetPackage, *FPaths::GetBaseFilename(ParentPackageName), RF_Standalone|RF_Public);
		NewParentMaterial->Parent = UnkownMaterial;
		FAssetRegistryModule::AssetCreated(NewParentMaterial);
		NewParentMaterial->Modify();
		NewParentMaterial->PostEditChange();
		ParentMaterial = NewParentMaterial;
	}
	UPackage* AssetPackage = CreatePackage(*GlobalMaterialInstanceName);

	UMaterialInstanceConstant* NewMaterial = NewObject<UMaterialInstanceConstant>(AssetPackage, *FPaths::GetBaseFilename(GlobalMaterialInstanceName), RF_Standalone|RF_Public);
	FAssetRegistryModule::AssetCreated(NewMaterial);
	NewMaterial->Parent = ParentMaterial;
	FRBMKEngineFactory	EngineFactory(nullptr,InFlags);
	FStaticParameterSet NewStaticParameterSet;

	TObjectPtr<UTexture2D> BaseTexture = EngineFactory.ImportTexture(TextureName);
	static FName NAME_Diffuse = "Diffuse";
	if (BaseTexture)
	{
		NewMaterial->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(NAME_Diffuse), BaseTexture);
	}

	
	NewMaterial->UpdateStaticPermutation(NewStaticParameterSet);
	NewMaterial->InitStaticPermutation();
	NewMaterial->Modify();
	NewMaterial->PostEditChange();
	return NewMaterial;
}

