#include "RBMKSceneSpawnPoint.h"

#include "StalkerEditorManager.h"
#include "Importer/Scene/Entitys/Shape//RBMKSceneShape.h"
#include "Managers/SEFactory/StalkerSEFactoryManager.h"

THIRD_PARTY_INCLUDES_START
#include "xrServerEntities/xrMessages.h"
THIRD_PARTY_INCLUDES_END

void FRMBKSceneSpawnData::Create(LPCSTR EntityReference)
{
	Destroy();
    if (GStalkerEditorManager->SEFactoryManager->IsVoid())
    {
        return;
    }
    ServerAbstract = GStalkerEditorManager->SEFactoryManager->CreateEntity(EntityReference);
    if (ServerAbstract)
    {
    	ServerAbstract->set_name	(EntityReference);
       
        if (pSettings->line_exist(ServerAbstract->name(),"$player"))
        {
            if (pSettings->r_bool(ServerAbstract->name(),"$player"))
            {
				ServerAbstract->flags().set(M_SPAWN_OBJECT_ASPLAYER,TRUE);
            }
        }
        ClassID = pSettings->r_clsid(ServerAbstract->name(),"class");
    }
	else
    {
        UE_LOG(LogXRayImporter,Warning,TEXT("Can't create entity:%S"),EntityReference);
    }
}

void FRMBKSceneSpawnData::Destroy()
{
    if (ServerAbstract)
    {
        GStalkerEditorManager->SEFactoryManager->DestroyEntity(ServerAbstract);
        ServerAbstract = nullptr;
    }
}

bool FRMBKSceneSpawnData::Valid() const
{
    return ServerAbstract!=nullptr;
}

bool FRMBKSceneSpawnData::LoadLTX	(CInifile& INIFile, const FString&SectionName)
{
    Create(INIFile.r_string(TCHAR_TO_ANSI(*SectionName), "name"));

    if(INIFile.line_exist(TCHAR_TO_ANSI(*SectionName),"fl"))
		Flags.assign		(INIFile.r_u8(TCHAR_TO_ANSI(*SectionName),"fl"));
        
    NET_Packet Packet;
    FRBMKSceneIniFileStream IniFileStream;
    IniFileStream.INIFile = &INIFile;
    IniFileStream.SectionName = TCHAR_TO_ANSI(*SectionName);
	IniFileStream.move_begin();
    Packet.inistream = &IniFileStream;
    
    if (Valid())
    {
    	if (!ServerAbstract->Spawn_Read(Packet))
    	{
    		Destroy		();
    	}
    }

    return Valid();
}

ISE_Abstract* FRMBKSceneSpawnData::GetEntity() const
{
    return ServerAbstract;
}

bool FRMBKSceneSpawnData::LoadStream(IReader& F)
{
    enum
    {
        SPAWNPOINT_CHUNK_ENTITYREF = 0xE419,
        SPAWNPOINT_CHUNK_SPAWNDATA = 0xE420,
        SPAWNPOINT_CHUNK_FLAGS = 0xE418,
    };

    string64 			BufferStr;
    if(!ensure(F.find_chunk(SPAWNPOINT_CHUNK_ENTITYREF)))
    {
	    return false;
    }
    F.r_stringZ			(BufferStr,sizeof(BufferStr));

    if(F.find_chunk(SPAWNPOINT_CHUNK_FLAGS))
    {
    	Flags.assign (F.r_u8());
    }

	if(!ensure(F.find_chunk(SPAWNPOINT_CHUNK_SPAWNDATA)))
	{
	   return false;
	}
    
    NET_Packet 			Packet;
    Packet.B.count= F.r_u32();
    F.r(Packet.B.data,Packet.B.count);
    Create(BufferStr);

    if (Valid())
    {
    	if (!ServerAbstract->Spawn_Read(Packet))
    	{
    		Destroy		();
    	}
    }
    return Valid();
}

FRMBKSceneSpawnData::FRMBKSceneSpawnData(FRBMKSceneSpawnPoint* InOwner):Owner(InOwner)
{
    ClassID	= 0;
	ServerAbstract = nullptr;
    Flags.zero();
}

FRMBKSceneSpawnData::~FRMBKSceneSpawnData()
{
	Destroy();
}

FRBMKSceneSpawnPoint::FRBMKSceneSpawnPoint(const FString&InName):FRBMKSceneObjectBase(InName),SpawnData(this)
{
    ObjectType = ERBMKSceneObjectType::SpawnPoint;
	
}
FRBMKSceneSpawnPoint::~FRBMKSceneSpawnPoint()
{
}

bool FRBMKSceneSpawnPoint::LoadLTX(CInifile& INIFile, const FString&SectionName)
{
    enum class EPointType
	{
		RPoint = 0,
		EnvMod,
		SpawnPoint,
	};
    const uint32 Version = INIFile.r_u32(TCHAR_TO_ANSI(*SectionName), "version");

    if(Version<0x0014)
    {
		UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneSpawnPoint: Unsupported version."));
        return false;
    }

	FRBMKSceneObjectBase::LoadLTX(INIFile, SectionName);

    const EPointType Type = static_cast<EPointType>(INIFile.r_u32(TCHAR_TO_ANSI(*SectionName), "type"));

    if (Type != EPointType::SpawnPoint)
    {
        UE_LOG(LogXRayImporter,Warning,TEXT("FRBMKSceneSpawnPoint: Unsupported spawn version."));
        return false;
    }

    const FString SpawnDataSectionName = SectionName + TEXT("_spawndata");
    if (!SpawnData.LoadLTX(INIFile, SpawnDataSectionName))
    {
        UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneSpawnPoint:Can't load Spawn Data."));
        return false;
    }

    if(INIFile.line_exist(TCHAR_TO_ANSI(*SectionName), "attached_count"))
    {
        FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
		AppendObjectDelegate.BindLambda([this](TSharedPtr<FRBMKSceneObjectBase> Object)
		{
		    check(!AttachedObject);
		    if (Object->GetObjectType() != ERBMKSceneObjectType::Shape)
		    {
				return;
		    }
		    AttachedObject = Object;
		    GRBMKScene->RemoveObject		(Object);

		});
    	GRBMKScene->ReadObjectsLTX(INIFile, TCHAR_TO_ANSI(*SectionName), "attached", AppendObjectDelegate);
    }

	// BUG fix
    if (const FRBMKSceneShape* Shape	= AttachedObject ? reinterpret_cast<FRBMKSceneShape*>(AttachedObject->QueryInterface(ERBMKSceneObjectType::Shape)) : nullptr)
    {
    	Transform.SetScale3D (Shape->GetTransform().GetScale3D());
    }
    return true;
}


bool FRBMKSceneSpawnPoint::LoadStream(IReader& F)
{
    
	enum
	{
	    SPAWNPOINT_CHUNK_VERSION	  = 0xE411,
	    SPAWNPOINT_CHUNK_ENTITYREF	  = 0xE419,
	    SPAWNPOINT_CHUNK_ATTACHED_OBJ = 0xE421,
	};

	uint16 Version = 0;

    if(!ensure(F.r_chunk(SPAWNPOINT_CHUNK_VERSION,&Version)))
    {
	    return false;
    }

    if(Version<0x0014)
    {
       UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneSpawnPoint: Unsupported version."));
        return false;
    }

	FRBMKSceneObjectBase::LoadStream(F);

    // new generation
    if (F.find_chunk(SPAWNPOINT_CHUNK_ENTITYREF))
    {
        if (!SpawnData.LoadStream(F))
        {
             UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneSpawnPoint:Can't load Spawn Data."));
            return false;
        }
    }
    else
    {
	    return false;
    }

    FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
	AppendObjectDelegate.BindLambda([this](TSharedPtr<FRBMKSceneObjectBase> Object)
	{
	    check(!AttachedObject);
	    if (Object->GetObjectType() != ERBMKSceneObjectType::Shape)
	    {
            return;
	    }
		AttachedObject = Object;
	    GRBMKScene->RemoveObject(Object);
	});
    GRBMKScene->ReadObjectsStream(F,SPAWNPOINT_CHUNK_ATTACHED_OBJ, AppendObjectDelegate);

	// BUG fix
	if (const FRBMKSceneShape* Shape	= AttachedObject ? reinterpret_cast<FRBMKSceneShape*>(AttachedObject->QueryInterface(ERBMKSceneObjectType::Shape)) : nullptr)
    {
    	Transform.SetScale3D (Shape->GetTransform().GetScale3D());
    }
    return true;
}
void* FRBMKSceneSpawnPoint::QueryInterface(ERBMKSceneObjectType InClassID)
{
	if (InClassID ==  ERBMKSceneObjectType::SpawnPoint)
		return this;
	return FRBMKSceneObjectBase::QueryInterface(InClassID);
}
