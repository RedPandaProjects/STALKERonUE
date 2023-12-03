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
    FClassID 			= ERBMKSceneObjectType::Unkown;

    FParentTools = 0;
    if (name) 	FName 	= name;
    m_pOwnerObject		= 0;
    ResetTransform		();
    FPosition.set		(0,0,0);
    FScale.set			(1,1,1);
    FRotation.set		(0,0,0);
}


FXRayCustomObject::~FXRayCustomObject()
{
}

void FXRayCustomObject::UpdateTransform()
{
	FTransformR.setXYZi		(-GetRotation().x, -GetRotation().y, -GetRotation().z);
                                
	FTransformS.scale		(GetScale());
	FTransformP.translate	(GetPosition());
	FTransformRP.mul		(FTransformP,FTransformR);
	FTransform.mul			(FTransformRP,FTransformS);
    FITransformRP.invert	(FTransformRP);
    FITransform.invert		(FTransform);

}

bool  FXRayCustomObject::LoadLTX(CInifile& ini, LPCSTR sect_name)
{;

	FName				= ini.r_string(sect_name, "name");
    FPosition			= ini.r_fvector3 	(sect_name, "position");
    VERIFY2				(_valid(FPosition), sect_name);
    FRotation			= ini.r_fvector3 	(sect_name, "rotation");
    VERIFY2				(_valid(FRotation), sect_name);
    FScale				= ini.r_fvector3 	(sect_name, "scale");
    VERIFY2				(_valid(FScale), sect_name);

	return true;
}

bool FXRayCustomObject::LoadStream(IReader& F)
{
    R_ASSERT(F.find_chunk(CUSTOMOBJECT_CHUNK_FLAGS));
    {
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

	UpdateTransform	();

	return true;
}
