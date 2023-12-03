#include "RBMKSceneObjectsToolBase.h"

FRBMKSceneObjectsToolBase::FRBMKSceneObjectsToolBase(ERBMKSceneObjectType cls):FRBMKSceneToolBase(cls)
{
}


FRBMKSceneObjectsToolBase::~FRBMKSceneObjectsToolBase()
{
}

TSharedPtr<FXRayCustomObject>  FRBMKSceneObjectsToolBase::FindObjectByName(LPCSTR name)
{
	for(TSharedPtr<FXRayCustomObject>& Object :Objects)
	{
		LPCSTR _name = Object->GetName();
		if (0 == FCStringAnsi::Strcmp(_name, name))
		{
			return Object;
		}
	}
	return nullptr;
}

bool FRBMKSceneObjectsToolBase::LoadSelection(IReader& F)
{
	static const u32 OTOOL_CHUNK_OBJECT_COUNT		= 0x0002;
	static const u32 OTOOL_CHUNK_OBJECTS			= 0x0003;
    int count					= 0;
	F.r_chunk					(OTOOL_CHUNK_OBJECT_COUNT,&count);

	FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
	AppendObjectDelegate.BindLambda([this](TSharedPtr<FXRayCustomObject> Object)
	{
		string256 				buf;
	    Scene->GenObjectName	(Object->FClassID,buf,Object->GetName());
	    Object->SetName(buf);
	    Scene->AppendObject		(Object);
	});

    Scene->ReadObjectsStream	(F,OTOOL_CHUNK_OBJECTS, AppendObjectDelegate);

    return true;
}

bool FRBMKSceneObjectsToolBase::LoadLTX(CInifile& ini)
{
	FRBMKSceneToolBase::LoadLTX	(ini);

    u32 count			= ini.r_u32("main", "objects_count");


    u32 i				= 0;
    string128			buff;

	for(i=0; i<count; ++i)
	{
		FCStringAnsi::Sprintf				(buff, "object_%d", i);
		if(TSharedPtr<FXRayCustomObject> Object = Scene->ReadObjectLTX(ini, buff) )
		{
			Scene->AppendObject	(Object);
		}
	}


    return true;
}

bool FRBMKSceneObjectsToolBase::LoadStream(IReader& F)
{
	FRBMKSceneToolBase::LoadStream		(F);

	static const u32 OTOOL_CHUNK_OBJECT_COUNT		= 0x0002;
	static const u32 OTOOL_CHUNK_OBJECTS			= 0x0003;

    int count					= 0;
	F.r_chunk					(OTOOL_CHUNK_OBJECT_COUNT,&count);

	FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
	AppendObjectDelegate.BindLambda([this](TSharedPtr<FXRayCustomObject> Object)
	{
		Scene->AppendObject	(Object);
	});

    Scene->ReadObjectsStream	(F,OTOOL_CHUNK_OBJECTS, AppendObjectDelegate);

    return true;
}

