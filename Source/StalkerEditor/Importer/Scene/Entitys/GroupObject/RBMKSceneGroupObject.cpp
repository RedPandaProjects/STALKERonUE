#include "RBMKSceneGroupObject.h"

FRBMKSceneGroupObject::FRBMKSceneGroupObject(const FString&Name):FRBMKSceneObjectBase(Name)
{
	ObjectType = ERBMKSceneObjectType::Group;
}

FRBMKSceneGroupObject::~FRBMKSceneGroupObject()
{
}

bool FRBMKSceneGroupObject::LoadLTX(CInifile& INIFile, const FString& SectionName)
{
	const uint32 Version = INIFile.r_u32(TCHAR_TO_ANSI(*SectionName), "version");
    if (Version < 0x0011)
    {
        UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneGroupObject: unsupported file version. Object can't load."));
        return false;
    }
	FRBMKSceneObjectBase::LoadLTX(INIFile, SectionName);

    Flags32 TempFlags;TempFlags.zero();
    if(Version<0x0012)
    {
    	TempFlags.assign(INIFile.r_u32(TCHAR_TO_ANSI(*SectionName), "flags") );
    }

    if(TempFlags.test((1<<0)))
    {    
        return false;
    }
	else
    {
		FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
    	AppendObjectDelegate.BindLambda([this](TSharedPtr<FRBMKSceneObjectBase> Object)
        {
			ObjectsInGroup.Add(Object);
			GRBMKScene->AppendObject				(Object);
		}
		);
	    GRBMKScene->ReadObjectsLTX(INIFile, SectionName, TEXT("ingroup"), AppendObjectDelegate);
    }
    return 			true;
}

void* FRBMKSceneGroupObject::QueryInterface(ERBMKSceneObjectType InClassID)
{
	if (InClassID == ERBMKSceneObjectType::Group)
		return this;
	return FRBMKSceneObjectBase::QueryInterface(InClassID);
}
bool FRBMKSceneGroupObject::LoadStream(IReader& F)
{
    enum
    {
		GROUPOBJ_CURRENT_VERSION		= 0x0012,
		GROUPOBJ_CHUNK_VERSION		  	= 0x0000,
		GROUPOBJ_CHUNK_OBJECT_LIST     	= 0x0001,
		GROUPOBJ_CHUNK_FLAGS	     	= 0x0003,
		GROUPOBJ_CHUNK_REFERENCE	  	= 0x0004,
		GROUPOBJ_CHUNK_OPEN_OBJECT_LIST	= 0x0005,
    };

    uint16 Version=0;
    if(!ensure(F.r_chunk(GROUPOBJ_CHUNK_VERSION,&Version)))
    {
	    return false;
    }
    if (Version<0x0011)
    {
        UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneGroupObject: unsupported file version. Object can't load."));
        return false;
    }

	FRBMKSceneObjectBase::LoadStream(F);

    Flags32 TempFlags;
	TempFlags.zero();

	if(Version<0x0012)
    {
    	F.r_chunk(GROUPOBJ_CHUNK_FLAGS,&TempFlags);
    }

    if (TempFlags.test(1<<0))
    {
        return false;
    }
	else
    {

		FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
    	AppendObjectDelegate.BindLambda([this](TSharedPtr<FRBMKSceneObjectBase> Object)
        {
			ObjectsInGroup.Add(Object);
			GRBMKScene->AppendObject(Object);
		}
		);
	    GRBMKScene->ReadObjectsStream(F,GROUPOBJ_CHUNK_OBJECT_LIST, AppendObjectDelegate);
    }

    return 			true;
}
