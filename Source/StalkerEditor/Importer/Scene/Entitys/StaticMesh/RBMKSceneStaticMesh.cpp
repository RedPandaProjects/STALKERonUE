#include "RBMKSceneStaticMesh.h"

FRBMKSceneStaticMesh::FRBMKSceneStaticMesh(const FString&InName):FRBMKSceneObjectBase(InName)
{
	ObjectType = ERBMKSceneObjectType::StaticMesh;
    ReferenceObject = nullptr;
}

FRBMKSceneStaticMesh::~FRBMKSceneStaticMesh()
{
    if(ReferenceObject)
    {
		GXRayObjectLibrary->RemoveEditObject(ReferenceObject);
    }
}

void* FRBMKSceneStaticMesh::QueryInterface(ERBMKSceneObjectType InClassID)
{
    if (InClassID == ERBMKSceneObjectType::StaticMesh)
        return this;
    return FRBMKSceneObjectBase::QueryInterface(InClassID);
}

bool FRBMKSceneStaticMesh::IsMUStatic() const
{
	if(ReferenceObject)
	{
		return !!ReferenceObject->IsMUStatic();
	}
    return false;
}

bool FRBMKSceneStaticMesh::IsStatic() const
{
    if(ReferenceObject)
	{
		return !!ReferenceObject->IsStatic();
	}
    return false;
}

CEditableObject* FRBMKSceneStaticMesh::GetReferenceObject() const
{
	return ReferenceObject;
}

void FRBMKSceneStaticMesh::UpdateReference()
{
    if(ReferenceObject)
    {
		GXRayObjectLibrary->RemoveEditObject(ReferenceObject);
    }
	ReferenceObject = ReferenceName.Len() > 0 ? GXRayObjectLibrary->CreateEditObject(TCHAR_TO_ANSI(*ReferenceName)) : nullptr;
    if (!ReferenceObject)
    {
		UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneStaticMesh:  '%s' not found in library."),*ReferenceName);
    }
}


bool FRBMKSceneStaticMesh::LoadLTX(CInifile& INIFile, const FString&SectionName)
{
	FRBMKSceneObjectBase::LoadLTX(INIFile, SectionName);
    ReferenceName = ANSI_TO_TCHAR(INIFile.r_string(TCHAR_TO_ANSI(*SectionName), "reference_name"));
    UpdateReference();
    return ReferenceObject != nullptr;
}

bool FRBMKSceneStaticMesh::LoadStream(IReader& F)
{
	enum
	{
		SCENEOBJ_CURRENT_VERSION	=	0x0012,
		SCENEOBJ_CHUNK_VERSION		=  	0x0900,
		SCENEOBJ_CHUNK_REFERENCE    = 	0x0902,
		SCENEOBJ_CHUNK_PLACEMENT    = 	0x0904,
		SCENEOBJ_CHUNK_FLAGS		=	0x0905,
		SCENEOBJ_CHUNK_SURFACE		=	0x0906
	};

	uint16 Version = 0;
	if(!ensure(F.r_chunk(SCENEOBJ_CHUNK_VERSION,&Version)))
	{
		return false;
	}
	if (Version==0x0010)
	{
		if(ensure(F.find_chunk(SCENEOBJ_CHUNK_PLACEMENT)))
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
			}; 
		}
	}

	FRBMKSceneObjectBase::LoadStream(F);
        
    if(!ensure(F.find_chunk(SCENEOBJ_CHUNK_REFERENCE)))
    {
	    return false;
    }
    if(Version<=0x0011)
    {
	    F.r_u32();
	    F.r_u32();
    }
    
	string1024 InReferenceName;
	F.r_stringZ	(InReferenceName,sizeof(InReferenceName));
	ReferenceName = ANSI_TO_TCHAR(InReferenceName);
    
    UpdateReference();
    return ReferenceObject != nullptr;
}