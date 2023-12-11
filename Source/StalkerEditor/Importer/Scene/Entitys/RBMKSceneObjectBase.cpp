#include "RBMKSceneObjectBase.h"

FRBMKSceneObjectBase::FRBMKSceneObjectBase(const FString&InName)
{
    ObjectType  =   ERBMKSceneObjectType::Unkown;
    Name = InName;
}

FRBMKSceneObjectBase::~FRBMKSceneObjectBase()
{
}

bool  FRBMKSceneObjectBase::LoadLTX(CInifile& INIFile, const FString&SectionName)
{;
	Name = ANSI_TO_TCHAR(INIFile.r_string(TCHAR_TO_ANSI(*SectionName), "name"));
    Fvector Position = INIFile.r_fvector3 	(TCHAR_TO_ANSI(*SectionName), "position");
    Fvector Rotation = INIFile.r_fvector3 	(TCHAR_TO_ANSI(*SectionName), "rotation");
    Fvector Scale	 = INIFile.r_fvector3 	(TCHAR_TO_ANSI(*SectionName), "scale");

    Fmatrix MTransformR,MTransformS,MTransformP,MTransformRP,MTransform;
	MTransformR.setXYZi		(-Rotation.x, -Rotation.y, -Rotation.z);                     
	MTransformS.scale		(Scale);
	MTransformP.translate	(Position);
	MTransformRP.mul		(MTransformP,MTransformR);
	MTransform.mul			(MTransformRP,MTransformS);
    Transform = FTransform(StalkerMath::RBMKMatrixToUnreal(MTransform));
	return true;
}

void* FRBMKSceneObjectBase::QueryInterface(ERBMKSceneObjectType InClassID)
{
	return nullptr;
}

bool FRBMKSceneObjectBase::LoadStream(IReader& F)
{
	enum
	{
		CUSTOMOBJECT_CHUNK_TRANSFORM = 0xF903,
		CUSTOMOBJECT_CHUNK_NAME		 = 0xF907,
	};

	if(ensure(F.find_chunk(CUSTOMOBJECT_CHUNK_NAME)))
    {
        shared_str InName;
        F.r_stringZ(InName);
        Name = ANSI_TO_TCHAR(InName.c_str());
    }

	if(F.find_chunk(CUSTOMOBJECT_CHUNK_TRANSFORM))
    {
		Fvector Position,Rotation,Scale;
        F.r_fvector3(Position);
        F.r_fvector3(Rotation);
        F.r_fvector3(Scale);
		{
		    Fmatrix MTransformR,MTransformS,MTransformP,MTransformRP,MTransform;
			MTransformR.setXYZi		(-Rotation.x, -Rotation.y, -Rotation.z);                     
			MTransformS.scale		(Scale);
			MTransformP.translate	(Position);
			MTransformRP.mul		(MTransformP,MTransformR);
			MTransform.mul			(MTransformRP,MTransformS);
		    Transform = FTransform(StalkerMath::RBMKMatrixToUnreal(MTransform));
	    }
    }
	return true;
}
