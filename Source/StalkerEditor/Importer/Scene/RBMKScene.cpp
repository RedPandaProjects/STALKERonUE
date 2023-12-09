#include "RBMKScene.h"

#include "Tools/AIMap/RBMKSceneAIMapTool.h"
#include "Tools/Detail/RBMKSceneDetailsObjectTool.h"
#include "Tools/Group/RBMKSceneGroupObjectsTool.h"
#include "Tools/Particles/RBMKSceneParticleSystemsTool.h"
#include "Tools/Shape/RBMKSceneShapesTool.h"
#include "Tools/Spawn/RBMKSpawnPointsTool.h"
#include "Tools/StaticMesh/RBMKSceneStaticMeshesTool.h"
#include "Tools/Wallmark/FRBMKSceneWallmarkTool.h"
#include "Tools/Way/RBMKSceneWayObjectsTool.h"

FRBMKScene*GRBMKScene = nullptr;
FRBMKScene::FRBMKScene(): SceneXRGUID()
{
	auto RegisterToolLambda = [this](const TSharedPtr<FRBMKSceneToolBase>& SceneToolBase)
	{
		SceneTools.Add(SceneToolBase->ObjectType, SceneToolBase);
	};
    
	RegisterToolLambda(MakeShared<FRBMKSceneStaticMeshesTool>());
	RegisterToolLambda(MakeShared<FRBMKSceneAIMapTool>());
	RegisterToolLambda(MakeShared<FRBMKSceneWayObjectsTool>());
	RegisterToolLambda(MakeShared<FRBMKSceneShapesTool>());
	RegisterToolLambda(MakeShared<FRBMKSpawnPointsTool>());
	RegisterToolLambda(MakeShared<FRBMKSceneGroupObjectsTool>());
	RegisterToolLambda(MakeShared<FRBMKSceneParticleSystemsTool>());
	RegisterToolLambda(MakeShared<FRBMKSceneWallmarkTool>());
	RegisterToolLambda(MakeShared<FRBMKSceneDetailsObjectTool>());
}

FRBMKScene::~FRBMKScene()
{
}

bool FRBMKScene::Load(const FString&FileName)
{
    enum
    {
	    CURRENT_FILE_VERSION = 0x0005,
	    CHUNK_TOOLS_GUID	 = 0x7000,
	    CHUNK_VERSION        = 0x9df3,
	    CHUNK_OBJECT_COUNT   = 0x7712,
	    CHUNK_OBJECT_LIST	 = 0x7708,
	    CHUNK_TOOLS_DATA	 = 0x8000,
    };

	UE_LOG(LogXRayImporter,Log,TEXT("FRBMKSceneScene: loading '%s'"), *FileName);

    if (FPaths::FileExists(FileName))
    {
        if(IsLTXFile(FileName))
        {
	        return LoadLTX(FileName);
        }
        IReader* FileData 	= FS.r_open(TCHAR_TO_ANSI(*FileName));

		uint32 Version = 0;
        if(!ensure(FileData->r_chunk(CHUNK_VERSION, &Version)))
        {
	        return false;
        }
        if (Version!=CURRENT_FILE_VERSION)
        {
            UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneScene: Unsupported file version. Can't load Level."));
            FS.r_close(FileData);
            return false;
        }

	    if (FileData->find_chunk(CHUNK_TOOLS_GUID))
        {
		    FileData->r			(&SceneXRGUID,sizeof(SceneXRGUID));
        }

      	FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
		AppendObjectDelegate.BindLambda([this](const TSharedPtr<FRBMKSceneObjectBase>& Object)
		{
			AppendObject	(Object);
		});
        ReadObjectsStream	(*FileData,CHUNK_OBJECT_LIST, AppendObjectDelegate);
        int32 ObjectCount = 0;
        for(auto&[Key,Value]:SceneTools)
        {
            if (Value)
            {
	            if (IReader* Chunk = FileData->open_chunk(CHUNK_TOOLS_DATA+static_cast<int32>(Key)))
                {
	                Value->LoadStream(*Chunk);
    	            Chunk->close	();
                }
            	else
                {
                    if (Key!=ERBMKSceneObjectType::AllTypes)
                    {
                    	const FString PartFileName = FPaths::GetBaseFilename(FileName,false)/Value->ClassName() + TEXT(".part");
                        LoadLevelPart	(Value.Get(),  PartFileName);
                        ObjectCount += Value->CastToSceneCustomOTool()?Value->CastToSceneCustomOTool()->Objects.Num():0;
                    }
                }
            }
		}
        
        UE_LOG(LogXRayImporter,Log,TEXT("FRBMKSceneScene: %d objects loaded"), ObjectCount);

		FS.r_close(FileData);
		return true;
    }
	else
    {
		UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneScene: Can't find file: '%s'"),*FileName);
    }
	return false;
}

bool FRBMKScene::LoadLTX(const FString&FileName)
{
	enum
    {
	    CURRENT_FILE_VERSION = 0x0005,
    };

	UE_LOG(LogXRayImporter,Log,TEXT("FRBMKSceneScene: Loading '%s'"), *FileName);
    if (FPaths::FileExists(*FileName))
    {

		CInifile INIFile(TCHAR_TO_ANSI(*FileName));
        const uint32 Version = INIFile.r_u32("version","value");
        if (Version != CURRENT_FILE_VERSION)
        {
        	UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneScene: Unsupported file version. Can't load Level."));
            return false;
        }

        SceneXRGUID.LoadLTX			(INIFile,"guid","guid");
        int32 ObjectCount = 0;
    	for(auto&[Key,Value]:SceneTools)
        {
            if (Value)
            {
                if ((Key!=ERBMKSceneObjectType::AllTypes))
                {
                    const FString PartFileName = FPaths::GetBaseFilename(FileName,false)/Value->ClassName() + TEXT(".part");
                    LoadLevelPartLTX	(Value.Get(), PartFileName);
                    ObjectCount += Value->CastToSceneCustomOTool()?Value->CastToSceneCustomOTool()->Objects.Num():0;
                }
            }
		}
        
        UE_LOG(LogXRayImporter,Log,TEXT("FRBMKSceneScene: %d objects loaded"), ObjectCount);
        
		return true;
    }
    else
    {
    	UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneScene: Can't find file: '%s'"),*FileName);
    }
	return false;
}


bool FRBMKScene::LoadSelection(const FString&FileName)
{
	enum
    {
	    CURRENT_FILE_VERSION = 0x0005,
	    CHUNK_TOOLS_GUID	 = 0x7000,
	    CHUNK_VERSION        = 0x9df3,
	    CHUNK_OBJECT_COUNT   = 0x7712,
	    CHUNK_OBJECT_LIST	 = 0x7708,
	    CHUNK_TOOLS_DATA	 = 0x8000,
    };

	UE_LOG(LogXRayImporter,Log,TEXT("FRBMKSceneScene: loading '%s'"), *FileName);
    bool Result = true;

   if (FPaths::FileExists(FileName))
    {
        IReader* FileData = FS.r_open(TCHAR_TO_ANSI(*FileName));

		uint32 Version = 0;
        if(!ensure(FileData->r_chunk(CHUNK_VERSION, &Version)))
        {
	        return false;
        }
        if (Version != CURRENT_FILE_VERSION)
        {
            UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneScene: Unsupported file version. Can't load Level."));
            FS.r_close(FileData);
            return false;
        }

    	FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
		AppendObjectDelegate.BindLambda([this](const TSharedPtr<FRBMKSceneObjectBase>& Object)
		{
		    AppendObject			(Object);
		});
      
        if (!ReadObjectsStream(*FileData,CHUNK_OBJECT_LIST, AppendObjectDelegate))
        {
			UE_LOG(LogXRayImporter,Error,TEXT("FRBMKScene. Failed to load selection."));
            Result = false;
        }

		for(auto&[Key,Value]:SceneTools)
		{
			if (Value)
            {
			    if (IReader* Chunk = FileData->open_chunk(CHUNK_TOOLS_DATA+static_cast<int32>(Key)))
			    {
			        Value->LoadSelection(*Chunk);
			        Chunk->close	();
			    }
			}
		}
		FS.r_close(FileData);
    }
	return Result;
}

FRBMKSceneToolBase* FRBMKScene::GetTool(ERBMKSceneObjectType ObjectType)
{
    if(const TSharedPtr<FRBMKSceneToolBase>*Tool = SceneTools.Find(ObjectType))
    {
	    return Tool->Get();
    }
    return nullptr;
}

FRBMKSceneObjectsToolBase* FRBMKScene::GetObjectTool(ERBMKSceneObjectType ObjectType)
{
    if(const TSharedPtr<FRBMKSceneToolBase>*Tool = SceneTools.Find(ObjectType))
    {
	    return Tool->Get()->CastToSceneCustomOTool();
    }
    return nullptr;
}


void FRBMKScene::AppendObject(const TSharedPtr<FRBMKSceneObjectBase>& Object )
{
	if(ensure(Object))
	{
		FRBMKSceneObjectsToolBase* SceneObjectsTool	= GetObjectTool(Object->GetObjectType());
		SceneObjectsTool->Objects.Add(Object);
	}
}

bool FRBMKScene::RemoveObject(const TSharedPtr<FRBMKSceneObjectBase>& Object )
{
	FRBMKSceneObjectsToolBase* SceneObjectsTool	=GetObjectTool(Object->GetObjectType());
    if (SceneObjectsTool&&Object)
    {
    	SceneObjectsTool->Objects.Remove(Object);
    }
    return true;
}

TSharedPtr<FRBMKSceneObjectBase>  FRBMKScene::FindObjectByName(const FString& Name, ERBMKSceneObjectType ObjectType)
{
	TSharedPtr<FRBMKSceneObjectBase> Result = nullptr;
	if (ObjectType == ERBMKSceneObjectType::AllTypes)
	{
		for(auto&[Key,Value]:SceneTools)
		{
			if(!Value)
			{
				continue;
			}

			if(FRBMKSceneObjectsToolBase* SceneCustomOTool = Value->CastToSceneCustomOTool())
			{
				Result = SceneCustomOTool->FindObjectByName(Name);
				if(Result)
				{
					return Result;
				}
			}
		}
	}
	else 
	{
		if(FRBMKSceneObjectsToolBase* SceneCustomOTool = GetObjectTool(ObjectType))
		{
			Result = SceneCustomOTool->FindObjectByName(Name);
			if(Result)
			{
				return Result;
			}
		}
	}
	return Result;
}

TSharedPtr<FRBMKSceneObjectBase>  FRBMKScene::FindObjectByName(const FString&Name)
{
	TSharedPtr<FRBMKSceneObjectBase>  Result = nullptr;
	for(auto&[Key,Value]:SceneTools)
	{
		if(!Value)
		{
			continue;
		}

		if(FRBMKSceneObjectsToolBase* SceneCustomOTool = Value->CastToSceneCustomOTool())
		{
			Result = SceneCustomOTool->FindObjectByName(Name);
			if(Result)
			{
				return Result;
			}
		}
	}
	return nullptr;
}

TSharedPtr<FRBMKSceneObjectBase> FRBMKScene::ReadObjectStream(IReader& F)
{
	constexpr uint32 CHUNK_OBJECT_CLASS = 0x7703;

	if(!ensure(F.find_chunk(CHUNK_OBJECT_CLASS)))
    {
	    return nullptr;
    }

	const ERBMKSceneObjectType ObjectType = static_cast<ERBMKSceneObjectType>(F.r_u32());

    if(FRBMKSceneObjectsToolBase* SceneObjectsToolBase = GetObjectTool(ObjectType))
    {
	    constexpr uint32 CHUNK_OBJECT_BODY = 0x7777;

	    TSharedPtr<FRBMKSceneObjectBase> NewObject = SceneObjectsToolBase->CreateObject(TEXT(""));
		IReader* ObjectData = F.open_chunk(CHUNK_OBJECT_BODY);
	    if(ensure(ObjectData))
	    {
		    const bool Result = NewObject->LoadStream(*ObjectData);
		    ObjectData->close();
			if (!Result)
		    {
		        return nullptr;
		    }
			return NewObject;
	    }
    }
	return nullptr;
}


TSharedPtr<FRBMKSceneObjectBase> FRBMKScene::ReadObjectLTX(CInifile& INIFile, const FString& SectionName)
{
	const ERBMKSceneObjectType ObjectType = static_cast<ERBMKSceneObjectType>(INIFile.r_u32(TCHAR_TO_ANSI(*SectionName),"clsid"));

    if(FRBMKSceneObjectsToolBase* SceneObjectsToolBase = GetObjectTool(ObjectType))
    {
	    TSharedPtr<FRBMKSceneObjectBase> NewObject = SceneObjectsToolBase->CreateObject(TEXT(""));

		if (!NewObject->LoadLTX(INIFile, SectionName))
		{
			return nullptr;
		}
        return NewObject;
    }
	return nullptr;
}

bool FRBMKScene::ReadObjectsStream(IReader& F, uint32 ChunkID, const FRMBKSceneAppendObjectDelegate& AppendObjectDelegate)
{
    bool Result = true;
    if (IReader* ObjectsChunk = F.open_chunk(ChunkID))
    {
        IReader* ObjectChunk = ObjectsChunk->open_chunk(0);
        for (int32 Count = 1; ObjectChunk; ++Count)
        {
            if (const TSharedPtr<FRBMKSceneObjectBase> Object = ReadObjectStream(*ObjectChunk))
            {
                AppendObjectDelegate.Execute(Object);
            }
            else 
            {
            	Result = false;
            }

            ObjectChunk->close();
            ObjectChunk = ObjectsChunk->open_chunk(Count);

        }
        ObjectsChunk->close();
    }
    return Result;
}

bool FRBMKScene::ReadObjectsLTX(CInifile& INIFile, const FString& SectionNameParent,  const FString& SectionNamePrefix, const FRMBKSceneAppendObjectDelegate& AppendObjectDelegate)
{
    string128 BufferStr;
    FCStringAnsi::Sprintf(BufferStr, "%s_count", TCHAR_TO_ANSI(*SectionNamePrefix));

    const uint32 Count = INIFile.r_u32(TCHAR_TO_ANSI(*SectionNameParent), BufferStr);

    bool Result = true;
    for (uint32 i = 0; i < Count; ++i)
    {
        FCStringAnsi::Sprintf(BufferStr, "%s_%s_%d", TCHAR_TO_ANSI(*SectionNameParent),  TCHAR_TO_ANSI(*SectionNamePrefix), i);
        if (const TSharedPtr<FRBMKSceneObjectBase> Object = ReadObjectLTX(INIFile, BufferStr))
        {
			AppendObjectDelegate.Execute(Object);
        }
        else 
        {
        	Result = false;
        }

    }
    return Result;
}

bool FRBMKScene::LoadLevelPartLTX(FRBMKSceneToolBase* SceneTool, const FString&FileName)
{
	if(!SceneTool->CanUseInifile())
	{
		return LoadLevelPartStream(SceneTool, FileName);
	}

    FString CurrentFileName = FileName;
    int32 FileIndex = 0;

    while(FPaths::FileExists(CurrentFileName))
    {
        if(!IsLTXFile(CurrentFileName))
        {
        	return LoadLevelPartStream(SceneTool, CurrentFileName);
        }


        CInifile			INIFile(TCHAR_TO_ANSI(*FileName));
        xrGUID				InGUID;
        InGUID.LoadLTX		(INIFile, "guid", "guid");

        if (InGUID!=SceneXRGUID)
        {
            UE_LOG(LogXRayImporter,Error,TEXT("FRBMKScene: Skipping invalid version of level part: '%s'"),*FileName);
            return 			FALSE;
        }
        SceneTool->LoadLTX			(INIFile);
        CurrentFileName = FString::Printf(TEXT("%s%d"),*FileName,++FileIndex);
    }

    return 					TRUE;
}

bool FRBMKScene::IsLTXFile(const FString& FileName)
{
	IReader* FileData = FS.r_open	(TCHAR_TO_ANSI(*FileName));
	if(ensure(FileData))
	{
		char OneChar;
	    FileData->r(&OneChar,sizeof(char));
		const bool IsLRX = (OneChar=='[');
	    FS.r_close(FileData);
        return IsLRX;
	}
    return false;
   
}

bool FRBMKScene::LoadLevelPart(FRBMKSceneToolBase* SceneTool, const FString&FileName)
{
	if(SceneTool->CanUseInifile())
	{
		return LoadLevelPartLTX(SceneTool, FileName);
	}
    return LoadLevelPartStream(SceneTool, FileName);
	
}

bool FRBMKScene::LoadLevelPartStream(FRBMKSceneToolBase* SceneTool, const FString&FileName)
{
    enum
    {
	    CURRENT_FILE_VERSION = 0x0005,
	    CHUNK_TOOLS_GUID	 = 0x7000,
	    CHUNK_VERSION        = 0x9df3,
	    CHUNK_OBJECT_COUNT   = 0x7712,
	    CHUNK_OBJECT_LIST	 = 0x7708,
	    CHUNK_TOOLS_DATA	 = 0x8000,
    };

    if (FPaths::FileExists(FileName))
    {
        IReader* FileData = FS.r_open(TCHAR_TO_ANSI(*FileName));
        if(!ensure(FileData->find_chunk(CHUNK_TOOLS_GUID)))
        {
			FS.r_close(FileData);
	        return false;
        }
        xrGUID InGUID;
        FileData->r(&InGUID, sizeof(InGUID));

        if (IReader* Chunk = FileData->open_chunk(CHUNK_TOOLS_DATA + static_cast<int32>(SceneTool->ObjectType)); Chunk != NULL)
        {
            SceneTool->LoadStream(*Chunk);
            Chunk->close();
        }
        else
        {
            UE_LOG(LogXRayImporter,Error,TEXT("FRBMKScene: Skipping invalid version of level part: '%s'"),*FileName);
            FS.r_close(FileData);
            return false;
        }
        //success
        FS.r_close(FileData);
        return true;
    }
    return false;
}


