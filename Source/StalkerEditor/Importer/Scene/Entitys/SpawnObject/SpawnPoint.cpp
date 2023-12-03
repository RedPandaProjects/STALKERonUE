#include "SpawnPoint.h"
#include "../ShapeObject/EShape.h"
#include "../../../../StalkerEditorManager.h"
#include "../../../../Managers/SEFactory/StalkerSEFactoryManager.h"
THIRD_PARTY_INCLUDES_START
#include "xrServerEntities/xrMessages.h"
THIRD_PARTY_INCLUDES_END

#define SPAWNPOINT_CHUNK_VERSION		0xE411
#define SPAWNPOINT_CHUNK_POSITION		0xE412
#define SPAWNPOINT_CHUNK_RPOINT			0xE413
#define SPAWNPOINT_CHUNK_DIRECTION		0xE414
#define SPAWNPOINT_CHUNK_SQUADID		0xE415
#define SPAWNPOINT_CHUNK_GROUPID		0xE416
#define SPAWNPOINT_CHUNK_TYPE			0xE417
#define SPAWNPOINT_CHUNK_FLAGS			0xE418

#define SPAWNPOINT_CHUNK_ENTITYREF		0xE419
#define SPAWNPOINT_CHUNK_SPAWNDATA		0xE420

#define SPAWNPOINT_CHUNK_ATTACHED_OBJ	0xE421

#define SPAWNPOINT_CHUNK_ENVMOD			0xE422
#define SPAWNPOINT_CHUNK_ENVMOD2		0xE423
#define SPAWNPOINT_CHUNK_ENVMOD3		0xE424
#define SPAWNPOINT_CHUNK_FLAGS			0xE425

// CLE_Visual
CLE_Visual::CLE_Visual(ISE_Visual* src)
{
	source				= src;
}

CLE_Visual::~CLE_Visual()
{
}

// CLE_Motion
CSpawnPoint::CLE_Motion::CLE_Motion	(ISE_Motion* src)
{
	source			= src;
    animator		= 0;
}
CSpawnPoint::CLE_Motion::~CLE_Motion()
{
}
void 	CSpawnPoint::CLE_Motion::OnChangeMotion	()
{

}
void CSpawnPoint::CLE_Motion::PlayMotion()
{
}
// SpawnData
void CSpawnPoint::SSpawnData::Create(LPCSTR _entity_ref)
{
	Destroy();
    if (GStalkerEditorManager->SEFactoryManager->IsVoid())
    {
        return;
    }
    m_Data 	= GStalkerEditorManager->SEFactoryManager->CreateEntity(_entity_ref);
    if (m_Data){
    	m_Data->set_name	(_entity_ref);
       
        if (pSettings->line_exist(m_Data->name(),"$player"))
        {
            if (pSettings->r_bool(m_Data->name(),"$player"))
            {
				m_Data->flags().set(M_SPAWN_OBJECT_ASPLAYER,TRUE);
            }
        }
        m_ClassID 			= pSettings->r_clsid(m_Data->name(),"class");
    }else{
    	Log("!Can't create entity: ",_entity_ref);
    }
}

void CSpawnPoint::SSpawnData::Destroy()
{
    if (m_Data)
    {
        GStalkerEditorManager->SEFactoryManager->DestroyEntity(m_Data);
        m_Data = nullptr;
    }
}
void CSpawnPoint::SSpawnData::get_bone_xform	(LPCSTR name, Fmatrix& xform)
{
	xform.identity		();
}

bool CSpawnPoint::SSpawnData::LoadLTX	(CInifile& ini, LPCSTR sect_name)
{
    xr_string temp 		= ini.r_string		(sect_name, "name");
    Create				(temp.c_str());

    if(ini.line_exist(sect_name,"fl"))
		m_flags.assign		(ini.r_u8(sect_name,"fl"));
        
    NET_Packet 				Packet;
    SIniFileStream 			ini_stream;
    ini_stream.ini 			= &ini;
    ini_stream.sect 		= sect_name;
	ini_stream.move_begin	();
    Packet.inistream 		= &ini_stream;
    
    if (Valid())
    	if (!m_Data->Spawn_Read(Packet))
        	Destroy		();

    return Valid();
}

bool CSpawnPoint::SSpawnData::LoadStream(IReader& F)
{
    string64 			temp;
    R_ASSERT			(F.find_chunk(SPAWNPOINT_CHUNK_ENTITYREF));
    F.r_stringZ			(temp,sizeof(temp));

    if(F.find_chunk(SPAWNPOINT_CHUNK_FLAGS))
		m_flags.assign	(F.r_u8());

    NET_Packet 			Packet;
    R_ASSERT(F.find_chunk(SPAWNPOINT_CHUNK_SPAWNDATA));
    Packet.B.count 		= F.r_u32();
    F.r					(Packet.B.data,Packet.B.count);
    Create				(temp);
    if (Valid())
    	if (!m_Data->Spawn_Read(Packet))
        	Destroy		();

    return Valid();
}

CSpawnPoint::CSpawnPoint(LPVOID data, LPCSTR name):FXRayCustomObject(data,name),m_SpawnData(this)
{
	m_rpProfile			= "";
    m_EM_Flags.one		();
	Construct			(data);
}

void CSpawnPoint::Construct(LPVOID data)
{
	FClassID			= ERBMKSceneObjectType::SpawnPoint;
    m_AttachedObject= 0;
    if (data)
    {
		CreateSpawnData(LPCSTR(data));
	}
}

CSpawnPoint::~CSpawnPoint()
{
	delete m_AttachedObject;
}


bool CSpawnPoint::AttachObject(FXRayCustomObject* obj)
{
	bool bAllowed = false;
    //  
    if (m_SpawnData.Valid()){
    	switch(obj->FClassID){
        case ERBMKSceneObjectType::Shape:
	    	bAllowed = !!m_SpawnData.m_Data->shape();
        break;
//        case ERBMKSceneObjectType::SceneObject:
//	    	bAllowed = !!dynamic_cast<xrSE_Visualed*>(m_SpawnData.m_Data);
//        break;
        }
    }
    //  
	if (bAllowed)
    {
        DetachObject				();
        OnAppendObject				(obj);
        m_AttachedObject->OnAttach	(this);
        SetPosition( m_AttachedObject->GetPosition());
        FRotation 	= m_AttachedObject->GetRotation();
        SetScale( 		 m_AttachedObject->GetScale());
    }
    return bAllowed;
}

void CSpawnPoint::DetachObject()
{
	if (m_AttachedObject)
    {
		m_AttachedObject->OnDetach();
        Scene->AppendObject(m_AttachedObject,false);
    	m_AttachedObject = 0;
    }
}

bool CSpawnPoint::CreateSpawnData(LPCSTR entity_ref)
{
	R_ASSERT(entity_ref&&entity_ref[0]);
    m_SpawnData.Destroy	();
    m_SpawnData.Create	(entity_ref);
    return m_SpawnData.Valid();
}
bool CSpawnPoint::OnAppendObject(FXRayCustomObject* object)
{
	R_ASSERT(!m_AttachedObject);
    if (object->FClassID!=ERBMKSceneObjectType::Shape) return false;
    // all right
    m_AttachedObject 		= object;
    object->m_pOwnerObject	= this;
    Scene->RemoveObject		(object, false, false);

    CEditShape* sh = reinterpret_cast<CEditShape*>(m_AttachedObject->QueryInterface(ERBMKSceneObjectType::Shape));
    if(m_SpawnData.Valid())
    {
        if(pSettings->line_exist(m_SpawnData.m_Data->name(),"shape_transp_color"))
        {
            sh->m_DrawTranspColor = pSettings->r_color(m_SpawnData.m_Data->name(),"shape_transp_color");
            sh->m_DrawEdgeColor = pSettings->r_color(m_SpawnData.m_Data->name(),"shape_edge_color");
        }
    }

    return true;
}
enum EPointType

{
	ptRPoint = 0,
	ptEnvMod,
	ptSpawnPoint,
	ptMaxType,
	pt_force_dword = u32(-1)
};

bool CSpawnPoint::LoadLTX(CInifile& ini, LPCSTR sect_name)
{

	u32 version = ini.r_u32(sect_name, "version");

    if(version<0x0014)
    {
        ELog.Msg( mtError, "SPAWNPOINT: Unsupported version.");
        return false;
    }

	FXRayCustomObject::LoadLTX(ini, sect_name);
    EPointType Type 			= (EPointType)ini.r_u32(sect_name, "type");

    if (Type != ptSpawnPoint)
    {
        ELog.Msg( mtError, "SPAWNPOINT: Unsupported spawn version.");
        return false;
    }
  
    string128	buff;
    strconcat	(sizeof(buff), buff, sect_name, "_spawndata");
    if (!m_SpawnData.LoadLTX(ini, buff))
    {
        ELog.Msg( mtError, "SPAWNPOINT: Can't load Spawn Data.");
        return false;
    }
	// objects
    if(ini.line_exist(sect_name, "attached_count"))
	    Scene->ReadObjectsLTX(ini, sect_name, "attached", EScene::TAppendObject(this, &CSpawnPoint::OnAppendObject));

	UpdateTransform	();

	// BUG fix
    CEditShape* shape	= m_AttachedObject ? reinterpret_cast<CEditShape*>(m_AttachedObject->QueryInterface(ERBMKSceneObjectType::Shape)):nullptr;
    if (shape)
    	SetScale 	( shape->GetScale());
    
    return true;
}


bool CSpawnPoint::LoadStream(IReader& F)
{
	u16 version = 0;

    R_ASSERT(F.r_chunk(SPAWNPOINT_CHUNK_VERSION,&version));
    if(version<0x0014)
    {
        ELog.Msg( mtError, "SPAWNPOINT: Unsupported version.");
        return false;
    }

	FXRayCustomObject::LoadStream(F);

    // new generation
    if (F.find_chunk(SPAWNPOINT_CHUNK_ENTITYREF))
    {
        if (!m_SpawnData.LoadStream(F))
        {
            ELog.Msg( mtError, "SPAWNPOINT: Can't load Spawn Data.");
            return false;
        }
    }
    else
    {
	    return false;
    }

	// objects
    Scene->ReadObjectsStream(F,SPAWNPOINT_CHUNK_ATTACHED_OBJ, EScene::TAppendObject(this, &CSpawnPoint::OnAppendObject));

    CEditShape* shape	= m_AttachedObject?reinterpret_cast<CEditShape*>(m_AttachedObject->QueryInterface(ERBMKSceneObjectType::Shape)):nullptr;
    if (shape) 	SetScale(shape->GetScale());
    
    return true;
}
void* CSpawnPoint::QueryInterface(ERBMKSceneObjectType InClassID)
{
	if (InClassID ==  ERBMKSceneObjectType::SpawnPoint)
		return this;
	return inherited::QueryInterface(InClassID);
}
