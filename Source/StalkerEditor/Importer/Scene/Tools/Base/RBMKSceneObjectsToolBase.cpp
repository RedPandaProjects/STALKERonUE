#include "RBMKSceneObjectsToolBase.h"

FRBMKSceneObjectsToolBase::FRBMKSceneObjectsToolBase(ERBMKSceneObjectType InObjectType):FRBMKSceneToolBase(InObjectType)
{
}


FRBMKSceneObjectsToolBase::~FRBMKSceneObjectsToolBase()
{
}

TSharedPtr<FRBMKSceneObjectBase>  FRBMKSceneObjectsToolBase::FindObjectByName(const FString&Name)
{
	for(TSharedPtr<FRBMKSceneObjectBase>& Object :Objects)
	{
		if (Name == Object->GetName())
		{
			return Object;
		}
	}
	return nullptr;
}

bool FRBMKSceneObjectsToolBase::LoadSelection(IReader& F)
{
	static constexpr uint32 OTOOL_CHUNK_OBJECT_COUNT	= 0x0002;
	static constexpr uint32 OTOOL_CHUNK_OBJECTS			= 0x0003;
    int32 Count					= 0;
	F.r_chunk					(OTOOL_CHUNK_OBJECT_COUNT,&Count);

	FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
	AppendObjectDelegate.BindLambda([this](TSharedPtr<FRBMKSceneObjectBase> Object)
	{
	    GRBMKScene->AppendObject		(Object);
	});

    GRBMKScene->ReadObjectsStream	(F,OTOOL_CHUNK_OBJECTS, AppendObjectDelegate);

    return true;
}

bool FRBMKSceneObjectsToolBase::LoadLTX(CInifile& ini)
{
	FRBMKSceneToolBase::LoadLTX	(ini);

	const uint32 Count			= ini.r_u32("main", "objects_count");
	for(uint32 i=0; i<Count; ++i)
	{
		string128 BufferStr;
		FCStringAnsi::Sprintf				(BufferStr, "object_%d", i);
		if(TSharedPtr<FRBMKSceneObjectBase> Object = GRBMKScene->ReadObjectLTX(ini, BufferStr) )
		{
			GRBMKScene->AppendObject	(Object);
		}
	}


    return true;
}

bool FRBMKSceneObjectsToolBase::LoadStream(IReader& F)
{
	FRBMKSceneToolBase::LoadStream		(F);

	static constexpr uint32 OTOOL_CHUNK_OBJECT_COUNT		= 0x0002;
	static constexpr uint32 OTOOL_CHUNK_OBJECTS			= 0x0003;

    int32 Count = 0;
	F.r_chunk(OTOOL_CHUNK_OBJECT_COUNT,&Count);

	FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
	AppendObjectDelegate.BindLambda([this](TSharedPtr<FRBMKSceneObjectBase> Object)
	{
		GRBMKScene->AppendObject	(Object);
	});

    GRBMKScene->ReadObjectsStream	(F,OTOOL_CHUNK_OBJECTS, AppendObjectDelegate);

    return true;
}

