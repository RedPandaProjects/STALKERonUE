#include "ParticlesObject.h"

#define CPSOBJECT_VERSION  				0x0013

#define CPSOBJECT_CHUNK_VERSION			0x0001
#define CPSOBJECT_CHUNK_REFERENCE		0x0002
#define CPSOBJECT_CHUNK_PARAMS			0x0003
#define CPSOBJECT_CHUNK_GAMETYPE		0x0004

CParticlesObject::CParticlesObject(LPVOID data, LPCSTR name):FXRayCustomObject(data,name)
{
	Construct	(data);
}

void CParticlesObject::Construct(LPVOID data)
{
	FClassID		= ERBMKSceneObjectType::ParticleSystem;
}

CParticlesObject::~CParticlesObject()
{
}


bool CParticlesObject::LoadLTX(CInifile& ini, LPCSTR sect_name)
{
    u32 version = ini.r_u32(sect_name,"version");

    inherited::LoadLTX(ini, sect_name);

    m_RefName		= ini.r_string(sect_name, "ref_name");
    return true;
}

void* CParticlesObject::QueryInterface(ERBMKSceneObjectType InClassID)
{
	if (InClassID == ERBMKSceneObjectType::ParticleSystem)
		return this;
	return inherited::QueryInterface(InClassID);
}

bool CParticlesObject::LoadStream(IReader& F)
{
  	u16 version = 0;

    R_ASSERT(F.r_chunk(CPSOBJECT_CHUNK_VERSION,&version));
    if(version<0x0011)
    {
        ELog.DlgMsg( mtError, "PSObject: Unsupported version.");
        return false;
    }

	inherited::LoadStream(F);

    R_ASSERT(F.find_chunk(CPSOBJECT_CHUNK_REFERENCE));

    F.r_stringZ(m_RefName);

    return true;
}
