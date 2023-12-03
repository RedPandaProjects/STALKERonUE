#include "CustomObject.h"

#define CUSTOMOBJECT_CHUNK_PARAMS 		0xF900
#define CUSTOMOBJECT_CHUNK_LOCK	 		0xF902
#define CUSTOMOBJECT_CHUNK_TRANSFORM	0xF903
#define CUSTOMOBJECT_CHUNK_GROUP		0xF904
#define CUSTOMOBJECT_CHUNK_MOTION		0xF905
#define CUSTOMOBJECT_CHUNK_FLAGS		0xF906
#define CUSTOMOBJECT_CHUNK_NAME			0xF907
#define CUSTOMOBJECT_CHUNK_MOTION_PARAM	0xF908


FXRayCustomObject::FXRayCustomObject(LPVOID data, LPCSTR name)
{
	save_id				= 0;
    FClassID 			= ERBMKSceneObjectType::Unkown;

    FParentTools = 0;
    if (name) 	FName 	= name;
    m_CO_Flags.assign	(0);
    m_RT_Flags.assign	(flRT_Valid|flRT_Visible);
    m_pOwnerObject		= 0;
    ResetTransform		();
    m_RT_Flags.set		(flRT_UpdateTransform,TRUE);
    m_Motion			= NULL;
    m_MotionParams 		= NULL;

    FPosition.set		(0,0,0);
    FScale.set			(1,1,1);
    FRotation.set		(0,0,0);
}


FXRayCustomObject::~FXRayCustomObject()
{
	delete m_Motion;
    delete m_MotionParams;
}

void FXRayCustomObject::OnUpdateTransform()
{

	m_RT_Flags.set			(flRT_UpdateTransform,FALSE);
    // update transform matrix

	FTransformR.setXYZi		(-GetRotation().x, -GetRotation().y, -GetRotation().z);
                                
	FTransformS.scale		(GetScale());
	FTransformP.translate	(GetPosition());
	FTransformRP.mul		(FTransformP,FTransformR);
	FTransform.mul			(FTransformRP,FTransformS);
    FITransformRP.invert	(FTransformRP);
    FITransform.invert		(FTransform);

    if (Motionable()&&Visible()&&Selected()&&m_CO_Flags.is(flAutoKey)) AnimationCreateKey(m_MotionParams->Frame());
}

BOOL   FXRayCustomObject::Editable() const 
{
	BOOL b1 = m_CO_Flags.is(flObjectInGroup);
    BOOL b2 = m_CO_Flags.is(flObjectInGroupUnique);
	return !b1 || (b1&&b2);
}

bool  FXRayCustomObject::LoadLTX(CInifile& ini, LPCSTR sect_name)
{
	m_CO_Flags.assign	(ini.r_u32(sect_name, "co_flags") );

	FName				= ini.r_string(sect_name, "name");
    FPosition			= ini.r_fvector3 	(sect_name, "position");
    VERIFY2				(_valid(FPosition), sect_name);
    FRotation			= ini.r_fvector3 	(sect_name, "rotation");
    VERIFY2				(_valid(FRotation), sect_name);
    FScale				= ini.r_fvector3 	(sect_name, "scale");
    VERIFY2				(_valid(FScale), sect_name);

    // object motion
    if (m_CO_Flags.is(flMotion))
    {
    	m_CO_Flags.set(flMotion, FALSE);
//    	R_ASSERT		(0);
/*
    	VERIFY			(m_Motion);
		F.open_chunk	(CUSTOMOBJECT_CHUNK_MOTION);
		m_Motion->Save	(F);
		F.close_chunk	();
  */
//        m_MotionParams->t_current = ini.r_float		(sect_name, "motion_params_t");
    }
	return true;
}

bool FXRayCustomObject::LoadStream(IReader& F)
{
    R_ASSERT(F.find_chunk(CUSTOMOBJECT_CHUNK_FLAGS));
    {
        m_CO_Flags.assign(F.r_u32());
    	
        R_ASSERT(F.find_chunk(CUSTOMOBJECT_CHUNK_NAME));
        F.r_stringZ		(FName);
    }

	if(F.find_chunk(CUSTOMOBJECT_CHUNK_TRANSFORM))
    {
        F.r_fvector3(FPosition);
        F.r_fvector3(FRotation);
        VERIFY(_valid(FRotation));
        F.r_fvector3(FScale);
    }

    // object motion
    if (F.find_chunk(CUSTOMOBJECT_CHUNK_MOTION))
    {
        m_Motion 	= xr_new<COMotion>();
        if (!m_Motion->Load(F)){
            ELog.Msg(mtError,"CustomObject: '%s' - motion has different version. Load failed.",GetName());
            delete m_Motion;
            m_Motion = nullptr;
        }
        m_MotionParams = xr_new<SAnimParams>();
	    m_MotionParams->Set(m_Motion);
        AnimationUpdate(m_MotionParams->Frame());
    }

    if (F.find_chunk(CUSTOMOBJECT_CHUNK_MOTION_PARAM)){
    	m_MotionParams->t_current = F.r_float();
        AnimationUpdate(m_MotionParams->Frame());
    }

	UpdateTransform	();

	return true;
}

void FXRayCustomObject::OnDetach()
{
	if (m_pOwnerObject) {
		m_pOwnerObject = 0;
		//.        string64 			new_name;
		//.        Scene->GenObjectName(ClassID,new_name,Name);
		//.        Name 				= new_name;
		//.        Scene->AppendObject	(this,false);
	}
	m_CO_Flags.set(flObjectInGroup, FALSE);
}

void FXRayCustomObject::OnAttach(FXRayCustomObject* owner)
{
	R_ASSERT(owner);
	R_ASSERT2(((!m_pOwnerObject) || (m_pOwnerObject == owner)), "Object already has owner!");
	m_pOwnerObject = owner;
	//.    Scene->RemoveObject		(this,false,false);
	if (owner->FClassID == ERBMKSceneObjectType::Group)
		m_CO_Flags.set(flObjectInGroup, TRUE);
}