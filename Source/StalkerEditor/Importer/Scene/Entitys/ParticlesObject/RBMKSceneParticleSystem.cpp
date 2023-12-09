#include "RBMKSceneParticleSystem.h"

FRBMKSceneParticleSystem::FRBMKSceneParticleSystem(const FString&Name):FRBMKSceneObjectBase(Name)
{
	ObjectType = ERBMKSceneObjectType::ParticleSystem;
}

FRBMKSceneParticleSystem::~FRBMKSceneParticleSystem()
{
}

bool FRBMKSceneParticleSystem::LoadLTX(CInifile& INIFile, const FString&SectionName)
{
    FRBMKSceneObjectBase::LoadLTX(INIFile, SectionName);
    ReferenceName = ANSI_TO_TCHAR(INIFile.r_string(TCHAR_TO_ANSI(*SectionName), "ref_name"));
    return true;
}

void* FRBMKSceneParticleSystem::QueryInterface(ERBMKSceneObjectType InObjectType)
{
	if (InObjectType == ERBMKSceneObjectType::ParticleSystem)
	{
		return reinterpret_cast<void*>(this);
	}
	return FRBMKSceneObjectBase::QueryInterface(InObjectType);
}

bool FRBMKSceneParticleSystem::LoadStream(IReader& F)
{
    enum
    {
	    CPSOBJECT_VERSION  			=	0x0013,
		CPSOBJECT_CHUNK_VERSION		=	0x0001,
		CPSOBJECT_CHUNK_REFERENCE	=	0x0002,
		CPSOBJECT_CHUNK_PARAMS		=	0x0003,
		CPSOBJECT_CHUNK_GAMETYPE	=	0x0004,
    };

  	uint16 Version = 0;

    if(!ensure(F.r_chunk(CPSOBJECT_CHUNK_VERSION,&Version)))
    {
	    return false;
    }

    if(Version<0x0011)
    {
        UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneParticleSystem: Unsupported version."));
        return false;
    }

	FRBMKSceneObjectBase::LoadStream(F);

    if(!ensure(F.find_chunk(CPSOBJECT_CHUNK_REFERENCE)))
    {
	    return false;
    }

    shared_str InReferenceName;
    F.r_stringZ(InReferenceName);
    ReferenceName = ANSI_TO_TCHAR(InReferenceName.c_str());

    return true;
}
