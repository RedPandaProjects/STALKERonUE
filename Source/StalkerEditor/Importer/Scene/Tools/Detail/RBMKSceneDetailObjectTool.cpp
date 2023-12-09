#include "RBMKSceneDetailObjectTool.h"

#include "RBMKDetailFormat.h"
#include "InstancedFoliageActor.h"
#include "Importer/Scene/Entitys/StaticObject/RBMKSceneStaticMesh.h"

THIRD_PARTY_INCLUDES_START
#include "xrEngine/cl_intersect.h"
THIRD_PARTY_INCLUDES_END

FRBMKSceneDetailObjectTool::FRBMKSceneDetailObjectTool():FRBMKSceneToolBase(ERBMKSceneObjectType::DetailObject)
{
    DetailHeader = {};
    InitRender();
}

FRBMKSceneDetailObjectTool::~FRBMKSceneDetailObjectTool()
{
}


void FRBMKSceneDetailObjectTool::ExportToWorld(UWorld*World,EObjectFlags InFlags,const UXRayLevelImportOptions&LevelImportOptions)
{
    auto AddInstances = [](UWorld* World, const UFoliageType* InFoliageType, const TArray<FTransform>& InTransforms)
	{
		TMap<AInstancedFoliageActor*, TArray<const FFoliageInstance*>> InstancesToAdd;
		TArray<FFoliageInstance> FoliageInstances;
		FoliageInstances.Reserve(InTransforms.Num()); // Reserve 

		for (const FTransform& InstanceTransform : InTransforms)
		{
			AInstancedFoliageActor* IFA = AInstancedFoliageActor::Get(World, true, World->PersistentLevel, InstanceTransform.GetLocation());
			FFoliageInstance FoliageInstance;
			FoliageInstance.Location = InstanceTransform.GetLocation();
			FoliageInstance.Rotation = InstanceTransform.GetRotation().Rotator();
			FoliageInstance.DrawScale3D = FVector3f(InstanceTransform.GetScale3D());

			FoliageInstances.Add(FoliageInstance);
			InstancesToAdd.FindOrAdd(IFA).Add(&FoliageInstances[FoliageInstances.Num() - 1]);
		}

		for (const auto& Pair : InstancesToAdd)
		{
			FFoliageInfo* TypeInfo = nullptr;
			if (const UFoliageType* FoliageType = Pair.Key->AddFoliageType(InFoliageType, &TypeInfo))
			{
				TypeInfo->AddInstances(FoliageType, Pair.Value);
			}
		}
	};

	DetailDensity = LevelImportOptions.DetailDensity;
    check(XRC);

	TArray<TArray<FTransform>> Instances;
    Instances.AddDefaulted(Objects.Num());
    for(int32 X = 0;X<static_cast<int32>(DetailHeader.SizeX);X++)
    {
	    for(int32 Z = 0;Z<static_cast<int32>(DetailHeader.SizeZ);Z++)
	    {
		    RenderSlot(X-DetailHeader.OffsetX,Z-DetailHeader.OffsetZ,Instances);
	    }
    }
	for(int32 i=0;i<Objects.Num();i++)
	{
        if(Instances[i].Num())
        {
			AddInstances(World,Objects[i]->GetOrCreateFoliageType(), Instances[i]);
        }
	}
}

void FRBMKSceneDetailObjectTool::InitRender()
{
    //--------------------------------------------------- Decompression
	static int32 Magic4x4[4][4] =
	{
 		{ 0, 14,  3, 13},
		{11,  5,  8,  6},
		{12,  2, 15,  1},
		{ 7,  9,  4, 10}
	};

	auto BWDitherMap =[]	(int32 levels, int32 magic[16][16])
	{
		/* Get size of each step */
	    float N = 255.0f / (levels - 1);

		/*
		* Expand 4x4 dither pattern to 16x16.  4x4 leaves obvious patterning,
		* and doesn't give us full intensity range (only 17 sublevels).
		*
		* magicfact is (N - 1)/16 so that we get numbers in the matrix from 0 to
		* N - 1: mod N gives numbers in 0 to N - 1, don't ever want all
		* pixels incremented to the next level (this is reserved for the
		* pixel value with mod N == 0 at the next level).
		*/

		const float	MagicFact = (N - 1) / 16;
	    for ( int32 i = 0; i < 4; i++ )
	    {
	    	for ( int32 j = 0; j < 4; j++ )
			{
				for ( int32 k = 0; k < 4; k++ )
				{
					for ( int32 l = 0; l < 4; l++ )
                    {
						magic[4*k+i][4*l+j] = FMath::TruncToInt32(0.5 + Magic4x4[i][j] * MagicFact + (Magic4x4[k][l] / 16.) * MagicFact);
                    }
				}
			}
	    }
	};
	// Make dither matrix
	BWDitherMap		(2,Dither);
}

bool FRBMKSceneDetailObjectTool::LoadColorIndices(IReader& F)
{
	enum
	{
	    DETMGR_CHUNK_OBJECTS 		= 0x0001ul,
	    DETMGR_CHUNK_BBOX			= 0x1001ul,
	    DETMGR_CHUNK_BASE_TEXTURE	= 0x1002ul,
	    DETMGR_CHUNK_COLOR_INDEX 	= 0x1003ul,
	    DETMGR_CHUNK_FLAGS			= 0x1006ul,
	};

    bool Result			= true;
    
    if (IReader* Chunk = F.open_chunk(DETMGR_CHUNK_OBJECTS))
    {
        IReader* Data   	= Chunk->open_chunk(0);
        for (int32 i =1; Data; i++) 
        {
            TSharedPtr<FRBMKDetail> NewDetail =  MakeShared<FRBMKDetail>();
            if (NewDetail->Load(*Data)) 
            {
            	Objects.Add(NewDetail);
            }
            else
            {
            	Result = false;
            }
            Data->close();
            Data = Chunk->open_chunk(i);
        }
        Chunk->close();
    }

    if(!ensure(F.find_chunk(DETMGR_CHUNK_COLOR_INDEX)))
    {
	    return false;
    }

    if(const int32  Count	= static_cast<int32>(F.r_u8()))
    {
	    for (int32 i = 0; i<Count; i++)
		{
			uint32 Index	= F.r_u32();
			const int32 ReferenceCount = static_cast<int32>(F.r_u8());
			for (int32 ReferenceID = 0; ReferenceID<ReferenceCount; ReferenceID++)
			{
				string256 BufferStr;
				F.r_stringZ	(BufferStr,sizeof(BufferStr));
		        if (FRBMKDetail* Detail	= FindDOByName(ANSI_TO_TCHAR(BufferStr)))
	            {
	            	ColorIndices.FindOrAdd(Index).Add(Detail);
	            }
	            else
	            {
	            	Result=false;
	            }
	        }
	    }
    }
    return Result;
}
bool FRBMKSceneDetailObjectTool::LoadLTX(CInifile& ini)
{
	checkNoEntry();
    return true;
}

bool FRBMKSceneDetailObjectTool::LoadStream(IReader& F)
{
	FRBMKSceneToolBase::LoadStream	(F);

	constexpr uint32 DETMGR_VERSION = 0x0003ul;
	
	enum
	{
	    DETMGR_CHUNK_VERSION		= 0x1000ul,
	    DETMGR_CHUNK_HEADER 		= 0x0000ul,
	    DETMGR_CHUNK_SLOTS			= 0x0002ul,
	    DETMGR_CHUNK_SNAP_OBJECTS 	= 0x1004ul,
	    DETMGR_CHUNK_DENSITY	 	= 0x1005ul,
	};

    if(!ensure(F.find_chunk(DETMGR_CHUNK_VERSION)))
    {
	    return false;
    }

	const uint32 Version = F.r_u32();
    if (Version != DETMGR_VERSION)
    {
		UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneDetailObjectTool: unsupported version."));
        return false;
    }
	// header
    if(!ensure(F.r_chunk(DETMGR_CHUNK_HEADER,&DetailHeader)))
    {
	    return false;
    }
    // slots
	if(!ensure(F.find_chunk(DETMGR_CHUNK_SLOTS)))
	{
	   return false;
	}
    DetailSlots.Empty();
    DetailSlots.AddZeroed( F.r_u32());
	F.r(DetailSlots.GetData(),DetailSlots.Num()*sizeof(FRBMKDetailSlot));

    // Objects
    if (!LoadColorIndices(F))
	{
		UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneDetailObjectTool: Some Objects removed. Reinitialize Objects."));
        InvalidateSlots	();
    }

	if (F.find_chunk(DETMGR_CHUNK_SNAP_OBJECTS))
    {
	    const int32 Count = F.r_s32();
        for (int32 i=0; i<Count; i++)
        {
	        string256 BufferStr;
	        F.r_stringZ	(BufferStr,sizeof(BufferStr));
	        TSharedPtr<FRBMKSceneObjectBase>  Object = GRBMKScene->FindObjectByName(BufferStr, ERBMKSceneObjectType::StaticMesh);
            if (!Object)	
            {
				UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneDetailObjectTool: Can't find snap object '%S'."),BufferStr);
            }
            else	
            {
            	SnapObjects.Add(Object);
            }
        }
    }
    return true;

}

bool FRBMKSceneDetailObjectTool::LoadSelection(IReader& F)
{
	return LoadStream			(F);
}


void FRBMKSceneDetailObjectTool::InvalidateSlots()
{
	for (FRBMKDetailSlot& DetailSlot : DetailSlots)
    {
    	DetailSlot.SetID(0,FRBMKDetailSlot::IDEmpty);
    	DetailSlot.SetID(1,FRBMKDetailSlot::IDEmpty);
    	DetailSlot.SetID(2,FRBMKDetailSlot::IDEmpty);
    	DetailSlot.SetID(3,FRBMKDetailSlot::IDEmpty);
    }
}

int FRBMKSceneDetailObjectTool::RemoveDOs()
{
	const int32 Num = Objects.Num();
    Objects.Empty();
    return Num;
}

FRBMKDetail* FRBMKSceneDetailObjectTool::FindDOByName(const TCHAR* Name)
{
	for(const TSharedPtr<FRBMKDetail>&Detail:Objects)
	{
		if(Detail->GetName() == Name)
		{
			return Detail.Get();
		}
	}
    return nullptr;
}

FRBMKDetailSlot&	FRBMKSceneDetailObjectTool::QueryDB(int32 X, int32 Z)
{
	const int32 XWithOffset = X+DetailHeader.OffsetX;
	const int32 ZWithOffset = Z+DetailHeader.OffsetZ;

	if ((XWithOffset>=0) && (XWithOffset<static_cast<int32>(DetailHeader.SizeX)) && 
		(ZWithOffset>=0) && (ZWithOffset<static_cast<int32>(DetailHeader.SizeZ)))
	{
		return DetailSlots				[ZWithOffset*DetailHeader.SizeX + XWithOffset];
	}
	else 
	{
		DetailSlotEmpty.SetID				(0,FRBMKDetailSlot::IDEmpty);
		DetailSlotEmpty.SetID				(1,FRBMKDetailSlot::IDEmpty);
		DetailSlotEmpty.SetID				(2,FRBMKDetailSlot::IDEmpty);
		DetailSlotEmpty.SetID				(3,FRBMKDetailSlot::IDEmpty);
		return DetailSlotEmpty;
	}
}


void		FRBMKSceneDetailObjectTool::RenderSlot(int32 X,int32 Z,TArray<TArray<FTransform>>&OutInstances)
{
	auto	Interpolate = [](const float * base, uint32 x, uint32 y, uint32 size)
	{
		const float	f	= static_cast<float>(size);
		const float	fx	= static_cast<float>(x)/f;
		const float ifx = 1.f-fx;
		const float	fy	= static_cast<float>(y)/f;
		const float ify = 1.f-fy;

		const float	c01	= base[0]*ifx + base[1]*fx;
		const float	c23	= base[2]*ifx + base[3]*fx;

		const float	c02	= base[0]*ify + base[2]*fy;
		const float	c13	= base[1]*ify + base[3]*fy;

		const float	cx	= ify*c01 + fy*c23;
		const float	cy	= ifx*c02 + fx*c13;
		return	(cx+cy)/2;
	};

	auto InterpolateAndDither = [Interpolate](const float* alpha255,	uint32 x, uint32 y, uint32 sx, uint32 sy, uint32 size, int32 dither[16][16] )
	{
		x = FMath::Clamp(x,0u,size-1);
		y = FMath::Clamp(y,0u,size-1);
		int32 c = FMath::FloorToInt(Interpolate(alpha255,x,y,size)+.5f);
		c  = FMath::Clamp(c,0,256);
		const uint32	row	= (y+sy) % 16;
		const uint32	col	= (x+sx) % 16;
		return	c	> dither[col][row];
	};

	FRBMKDetailSlot&DetailSlot = QueryDB(X,Z);

	Fbox BoundBox;

	BoundBox.min.set(X*DetailSlotSize, DetailSlot.GetYBase(),Z*DetailSlotSize);
	BoundBox.max.set(BoundBox.min.x+DetailSlotSize,DetailSlot.GetYBase()+DetailSlot.GetYHeight(),BoundBox.min.z+DetailSlotSize);
	BoundBox.grow(EPS_L);

	Fvector BoundBoxCenter,BoundBoxExtent;
	BoundBox.get_CD	(BoundBoxCenter,BoundBoxExtent);

	XRC->box_options(CDB::OPT_FULL_TEST);
	// Select polygons
	
	SBoxPickInfoVec		PickInfos;

	for(TSharedPtr<FRBMKSceneObjectBase>& Object :SnapObjects)
	{
		if(FRBMKSceneStaticMesh* SceneObject =  reinterpret_cast<FRBMKSceneStaticMesh*>(Object->QueryInterface(ERBMKSceneObjectType::StaticMesh)))
		{
			if(SceneObject->GetReferenceObject())
			{
				SceneObject->GetReferenceObject()->BoxPick(SceneObject, BoundBox,StalkerMath::UnrealMatrixToXRay(SceneObject->GetTransform().ToMatrixWithScale()), PickInfos);
			}
		}
	}
   
	uint32	TriCount		= PickInfos.size();
	if (0==TriCount)
	{
		return;
	}


	float		alpha255	[DetailObjectsInSlot][4];
	for (int i=0; i<DetailObjectsInSlot; i++)
	{
		alpha255[i][0]	= 255.f*static_cast<float>(DetailSlot.Palette[i].Alpha0)/15.f;
		alpha255[i][1]	= 255.f*static_cast<float>(DetailSlot.Palette[i].Alpha1)/15.f;
		alpha255[i][2]	= 255.f*static_cast<float>(DetailSlot.Palette[i].Alpha2)/15.f;
		alpha255[i][3]	= 255.f*static_cast<float>(DetailSlot.Palette[i].Alpha3)/15.f;
	}
	const int32 SendRnd					= X*Z;
	CRandom				RandomSelection	(0x12071980^SendRnd);
	CRandom				RandomJitter	(0x12071980^SendRnd);
	CRandom				RandomYaw		(0x12071980^SendRnd);
	CRandom				RandomScale		(0x12071980^SendRnd);

	float		Jitter				= DetailDensity/1.7f;
	int32		DSize				= FMath::CeilToInt(DetailSlotSize/DetailDensity);

	TArray<int32> Selected;
	Selected.Empty(DetailObjectsInSlot);

	for (int32 z=0; z<=DSize; z++)
	{
		for (int32 x=0; x<=DSize; x++)
		{
			const int32 ShiftX =  RandomJitter.randI(16);
			const int32 ShiftZ =  RandomJitter.randI(16);

			if ((DetailSlot.ID0!=FRBMKDetailSlot::IDEmpty)&& InterpolateAndDither(alpha255[0],x,z,ShiftX,ShiftZ,DSize,Dither))	Selected.Add(0);
			if ((DetailSlot.ID1!=FRBMKDetailSlot::IDEmpty)&& InterpolateAndDither(alpha255[1],x,z,ShiftX,ShiftZ,DSize,Dither))	Selected.Add(1);
			if ((DetailSlot.ID2!=FRBMKDetailSlot::IDEmpty)&& InterpolateAndDither(alpha255[2],x,z,ShiftX,ShiftZ,DSize,Dither))	Selected.Add(2);
			if ((DetailSlot.ID3!=FRBMKDetailSlot::IDEmpty)&& InterpolateAndDither(alpha255[3],x,z,ShiftX,ShiftZ,DSize,Dither))	Selected.Add(3);
			
			// Select
			if (Selected.IsEmpty())	continue;

			int32 Index = INDEX_NONE;
			if (Selected.Num()==1)
			{
				Index = Selected[0];
			}
			else
			{
				Index = Selected[RandomSelection.randI(Selected.Num())];
			}

			int32 ObjectIndex = static_cast<int32>(DetailSlot.GetID(Index));
			TSharedPtr<FRBMKDetail>	ObjectPtr= Objects[ObjectIndex];

			const float		WorldX = (static_cast<float>(x)/static_cast<float>(DSize))*DetailSlotSize + BoundBox.min.x;
			const float		WorldZ = (static_cast<float>(z)/static_cast<float>(DSize))*DetailSlotSize + BoundBox.min.z;
			Fvector		WorldPosition(WorldX + RandomJitter.randFs(Jitter), BoundBox.max.y, WorldZ + RandomJitter.randFs(Jitter));

			// Position (Y)
			float WorldY = BoundBox.min.y-5;
			Fvector	RayDirection(0,-1,0);

			float RayU,RayV,RayRange;
			for (uint32 TriID=0; TriID<TriCount; TriID++)
			{
				SBoxPickInfo& I=PickInfos[TriID];
				for (size_t k=0; k<I.inf.size(); k++)
				{
					Fvector Vertices[3];
					checkSlow(I.s_obj);
					I.e_obj->GetFaceWorld(StalkerMath::UnrealMatrixToXRay(I.s_obj->GetTransform().ToMatrixWithScale()),I.e_mesh,I.inf[k].id,Vertices);
					if (CDB::TestRayTri(WorldPosition,RayDirection,Vertices,RayU,RayV,RayRange,TRUE))
					{
						if (RayRange>=0)	
						{
							float y_test	= WorldPosition.y - RayRange;
							if (y_test>WorldY)	WorldY = y_test;
						}
					}
				}
			}
			if (WorldY<BoundBox.min.y)			continue;
			WorldPosition.y	= WorldY;

			float Scale = RandomScale.randF		(ObjectPtr->MinScale*0.5f,ObjectPtr->MaxScale*0.9f);

			Fmatrix MatrixScale,XForm, MatrixRotate;

			MatrixRotate.rotateY(RandomYaw.randF(0,PI_MUL_2));
			MatrixRotate.translate_over	(WorldPosition);
			MatrixScale.scale(Scale,Scale,Scale);
			XForm.mul_43(MatrixRotate,MatrixScale);
	
			FTransform Transform;
			Transform.SetFromMatrix(StalkerMath::XRayMatrixToUnreal(XForm));
			OutInstances[ObjectIndex].Add(Transform);
		}
	}
}
