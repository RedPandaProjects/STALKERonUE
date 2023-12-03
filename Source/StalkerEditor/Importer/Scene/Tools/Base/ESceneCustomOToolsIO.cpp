#include "ESceneCustomOTools.h"

// chunks

static const u32 OTOOL_CHUNK_OBJECT_COUNT		= 0x0002;
static const u32 OTOOL_CHUNK_OBJECTS			= 0x0003;
static const u32 OTOOL_CHUNK_FLAGS			= 0x0004;


bool FXRaySceneCustomOTool::LoadSelection(IReader& F)
{
    int count					= 0;
	F.r_chunk					(OTOOL_CHUNK_OBJECT_COUNT,&count);

	FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
	AppendObjectDelegate.BindLambda([this](FXRayCustomObject* Object)
	{
		string256 				buf;
	    Scene->GenObjectName	(Object->FClassID,buf,Object->GetName());
	    Object->SetName(buf);
	    Scene->AppendObject		(Object, false);
	});

    Scene->ReadObjectsStream	(F,OTOOL_CHUNK_OBJECTS, AppendObjectDelegate);

    return true;
}



bool FXRaySceneCustomOTool::LoadLTX(CInifile& ini)
{
	inherited::LoadLTX	(ini);

    u32 count			= ini.r_u32("main", "objects_count");


    u32 i				= 0;
    string128			buff;

	for(i=0; i<count; ++i)
	{
		FXRayCustomObject* obj	= NULL;
		FCStringAnsi::Sprintf				(buff, "object_%d", i);
		if( Scene->ReadObjectLTX(ini, buff, obj) )
		{
			Scene->AppendObject	(obj,false);
		}
	}


    return true;
}

bool FXRaySceneCustomOTool::LoadStream(IReader& F)
{
	inherited::LoadStream		(F);

    int count					= 0;
	F.r_chunk					(OTOOL_CHUNK_OBJECT_COUNT,&count);

	FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
	AppendObjectDelegate.BindLambda([this](FXRayCustomObject* Object)
	{
		Scene->AppendObject	(Object,false);
	});

    Scene->ReadObjectsStream	(F,OTOOL_CHUNK_OBJECTS, AppendObjectDelegate);

    return true;
}

