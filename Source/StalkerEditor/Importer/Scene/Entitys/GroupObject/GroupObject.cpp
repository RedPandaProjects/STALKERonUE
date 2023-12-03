#include "GroupObject.h"

#define GROUPOBJ_CURRENT_VERSION		0x0012

#define GROUPOBJ_CHUNK_VERSION		  	0x0000
#define GROUPOBJ_CHUNK_OBJECT_LIST     	0x0001
#define GROUPOBJ_CHUNK_FLAGS	     	0x0003
#define GROUPOBJ_CHUNK_REFERENCE	  	0x0004
#define GROUPOBJ_CHUNK_OPEN_OBJECT_LIST	0x0005

CGroupObject::CGroupObject(LPVOID data, LPCSTR name):FXRayCustomObject(data,name)
{
	Construct	(data);
}

void CGroupObject::Construct(LPVOID data)
{
	FClassID		= ERBMKSceneObjectType::Group;
}

CGroupObject::~CGroupObject()
{
}

void CGroupObject::ClearInternal(xr_list<FXRayCustomObject*>& Objects)
{
    for (FXRayCustomObject*Object:Objects)
    {
        Object->m_pOwnerObject		= 0;
        delete  Object;
    }
    Objects.clear();
}



void  CGroupObject::SetRefName(LPCSTR nm)
{
	m_GroupReferenceName = nm;
    if(nm && strstr(nm, "light_preset"))
    	save_id = 1;
    else
    	save_id = 0;
}


bool CGroupObject::LoadLTX(CInifile& ini, LPCSTR sect_name)
{
    u32 version = ini.r_u32(sect_name, "version");
    if (version<0x0011)
    {
        ELog.DlgMsg( mtError, "CGroupObject: unsupported file version. Object can't load.");
        return false;
    }
	FXRayCustomObject::LoadLTX(ini, sect_name);

    Flags32 tmp_flags;tmp_flags.zero();
    if(version<0x0012)
    	tmp_flags.assign(ini.r_u32(sect_name, "flags") );
    if(tmp_flags.test((1<<0)))
    {    
        return false;
    }else
    {
        
		FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
    	AppendObjectDelegate.BindLambda([this](TSharedPtr<FXRayCustomObject> Object)
        {
		    Object->m_pOwnerObject			= this;
		    Object->m_CO_Flags.set			(flObjectInGroup, TRUE);
		    m_ObjectsInGroup.Add(Object);


		    string256 			buf;
		    Scene->GenObjectName(Object->FClassID, buf, Object->GetName());
		    LPCSTR N = Object->GetName();
		    if (xr_strcmp(N,buf)!=0)
		    {
       			Msg					("Load_Append name changed from[%s] to[%s]",Object->GetName(), buf);
		        Object->SetName(buf);
		    }

			Scene->AppendObject				(Object);
		}
		);
	    Scene->ReadObjectsLTX			(ini, sect_name, "ingroup", AppendObjectDelegate);
    }
    VERIFY(m_ObjectsInGroup.size());

   	SetRefName(ini.r_string	(sect_name, "ref_name")) ;
	if (!m_GroupReferenceName.size())
        ELog.Msg			(mtError,"ERROR: group '%s' - has empty reference. Corrupted file?", GetName());
    

    if(version<0x0012)
    {
        for (TSharedPtr<FXRayCustomObject>& Obect :m_ObjectsInGroup)
        {
			Obect->m_CO_Flags.set(flObjectInGroup, TRUE);
        	Obect->m_CO_Flags.set(flObjectInGroupUnique, TRUE);
        }
    }

    return 			true;
}

void* CGroupObject::QueryInterface(ERBMKSceneObjectType InClassID)
{
	if (InClassID == ERBMKSceneObjectType::Group)
		return this;
	return inherited::QueryInterface(InClassID);
}
bool CGroupObject::LoadStream(IReader& F)
{
    u16 version=0;

    R_ASSERT(F.r_chunk(GROUPOBJ_CHUNK_VERSION,&version));
    if (version<0x0011)
    {
        ELog.DlgMsg( mtError, "CGroupObject: unsupported file version. Object can't load.");
        return false;
    }
	FXRayCustomObject::LoadStream(F);

    Flags32 tmp_flags; tmp_flags.zero();
    if(version<0x0012)
    	F.r_chunk(GROUPOBJ_CHUNK_FLAGS,&tmp_flags);

	// objects
    if (tmp_flags.test(1<<0))
    { //old format, opened group
        //ELog.DlgMsg( mtError, "old format, opened group");
        return false;
/*        
        R_ASSERT(F.find_chunk(GROUPOBJ_CHUNK_OPEN_OBJECT_LIST));
        u32 cnt 	= F.r_u32();
        for (u32 k=0; k<cnt; ++k)
        {
			m_ObjectsInGroup.resize	(m_ObjectsInGroup.size()+1);
            F.r_stringZ				(m_ObjectsInGroup.back().ObjectName);
        }
*/        
    }
	else
    {

		FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
    	AppendObjectDelegate.BindLambda([this](TSharedPtr<FXRayCustomObject> Object)
        {
		    Object->m_pOwnerObject			= this;
		    Object->m_CO_Flags.set			(flObjectInGroup, TRUE);
		    m_ObjectsInGroup.Add(Object);


		    string256 			buf;
		    Scene->GenObjectName(Object->FClassID, buf, Object->GetName());
		    LPCSTR N = Object->GetName();
		    if (xr_strcmp(N,buf)!=0)
		    {
       			Msg					("Load_Append name changed from[%s] to[%s]",Object->GetName(), buf);
		        Object->SetName(buf);
		    }

			Scene->AppendObject				(Object);
		}
		);
	    Scene->ReadObjectsStream(F,GROUPOBJ_CHUNK_OBJECT_LIST, AppendObjectDelegate);
    }
    VERIFY(m_ObjectsInGroup.size());

    if (F.find_chunk(GROUPOBJ_CHUNK_REFERENCE))	
    {
    	shared_str rn;
    	F.r_stringZ	(rn);
        SetRefName(rn.c_str());
     }
     
    if(version<0x0012)
    {
        for (TSharedPtr<FXRayCustomObject>& Obect :m_ObjectsInGroup)
        {
            if(Obect)
            {
            	Obect->m_CO_Flags.set(flObjectInGroup, TRUE);
            	Obect->m_CO_Flags.set(flObjectInGroupUnique, TRUE);
            }
        }
    }

    return 			true;
}

void CGroupObject::OnUpdateTransform()
{
	inherited::OnUpdateTransform();
	 for (TSharedPtr<FXRayCustomObject>&Obect:m_ObjectsInGroup)
    	Obect->OnUpdateTransform();
}
