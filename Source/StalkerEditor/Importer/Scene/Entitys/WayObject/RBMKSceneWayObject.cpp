#include "RBMKSceneWayObject.h"

FRBMKSceneWayPoint::FRBMKSceneWayPoint(const FString&InName):Name(InName)
{
    Flags = 0;
}

FRBMKSceneWayPoint::~FRBMKSceneWayPoint()
{
}

void FRBMKSceneWayPoint::CreateLink(TWeakPtr<FRBMKSceneWayPoint> Point, float Probability)
{
    if(Point.Pin().Get() != this)
    {
	    Links.Add(FRBMKSceneWayPointLink(Point,Probability));
    }
}
bool FRBMKSceneWayPoint::AppendLink(TWeakPtr<FRBMKSceneWayPoint> Point, float Probability)
{
	if (!FindLink(Point))
    {
    	CreateLink(Point,Probability);
        return true;
    }
    return false;
}
bool FRBMKSceneWayPoint::DeleteLink(TWeakPtr<FRBMKSceneWayPoint> Point)
{
    if(const FRBMKSceneWayPointLink*Link = FindLink(Point))
    {
		Links.Remove(*Link);
        return true;
    }
    return false;
}

FRBMKSceneWayPointLink* FRBMKSceneWayPoint::FindLink(TWeakPtr<FRBMKSceneWayPoint> Point)
{
	return Links.FindByPredicate([Point](const FRBMKSceneWayPointLink&Link){return Link.WayPoint == Point;});
}

FRBMKSceneWayObject::FRBMKSceneWayObject(const FString&Name):FRBMKSceneObjectBase(Name)
{
	ObjectType = ERBMKSceneObjectType::Way;
}


FRBMKSceneWayObject::~FRBMKSceneWayObject()
{
}

bool FRBMKSceneWayObject::LoadLTX(CInifile& INIFile, const FString&SectionName)
{
	const u32 Version 	= INIFile.r_u32(TCHAR_TO_ANSI(*SectionName), "version");

    if(Version != WayObjectVersion)
    {
    	UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneWayPoint: Unsupported version."));
        return false;
    }

	FRBMKSceneObjectBase::LoadLTX(INIFile, SectionName);

    if(GetName().IsEmpty())
	{
        UE_LOG(LogXRayImporter,Error,TEXT("Corrupted scene file.[%S] sect[%s] has empty name"),INIFile.fname(),*SectionName);
		return false;
    }

	const uint32 WayPointsCount = INIFile.r_u32(TCHAR_TO_ANSI(*SectionName), "wp_count");
    for(uint32 i = 0;i<WayPointsCount;i++)
    {
        FString KeyName = FString::Printf(TEXT("wp_%u_name"),i);
        FString InName = ANSI_TO_TCHAR(INIFile.r_string(TCHAR_TO_ANSI(*SectionName), TCHAR_TO_ANSI(*KeyName)));
        const TSharedPtr<FRBMKSceneWayPoint>& WayPoint = WayPoints.Add_GetRef(MakeShared<FRBMKSceneWayPoint>(TCHAR_TO_ANSI(*InName)));
    	FString KeyPosition = FString::Printf(TEXT("wp_%u_pos"),i);
        WayPoint->Position = FVector(StalkerMath::RBMKLocationToUnreal(INIFile.r_fvector3(TCHAR_TO_ANSI(*SectionName), TCHAR_TO_ANSI(*KeyPosition))));
    	FString KeyFlags = FString::Printf(TEXT("wp_%u_flags"),i);
        WayPoint->Flags = INIFile.r_u32(TCHAR_TO_ANSI(*SectionName), TCHAR_TO_ANSI(*KeyFlags));
    }
    CInifile::Sect& Section 		= INIFile.r_section(TCHAR_TO_ANSI(*SectionName));
    for(CInifile::Item& Item :Section.Data)
    {
    	if( Item.first.c_str() == FCStringAnsi::Strstr(Item.first.c_str(),"link_wp_") )
        {
        	int32 WayPointIndex = INDEX_NONE;
        	int32 WayPointLinkIndex = INDEX_NONE;

	        const int32 Result = sscanf_s(Item.first.c_str(),"link_wp_%4d_%4d",&WayPointIndex, &WayPointLinkIndex);
            if(ensure(Result==2))
            {
            	Fvector2 Value = INIFile.r_fvector2(TCHAR_TO_ANSI(*SectionName),Item.first.c_str());
	            const int32 WayPointToIndex = FMath::FloorToInt(Value.x);
        		WayPoints[WayPointIndex]->CreateLink(WayPoints[WayPointToIndex], Value.y);
            }
        }
    }

    return true;
}

void* FRBMKSceneWayObject::QueryInterface(ERBMKSceneObjectType InClassID)
{
	if (InClassID == ERBMKSceneObjectType::Way)
		return this;
	return FRBMKSceneObjectBase::QueryInterface(InClassID);
}

bool FRBMKSceneWayObject::LoadStream(IReader& F)
{
    enum
    {
	    WAYOBJECT_CHUNK_VERSION	= 0x0001,
		WAYOBJECT_CHUNK_POINTS	= 0x0002,
		WAYOBJECT_CHUNK_LINKS	= 0x0003,
		WAYOBJECT_CHUNK_TYPE	= 0x0004,
		WAYOBJECT_CHUNK_NAME	= 0x0005,
    };

	uint16 Version = 0;

    if (!F.find_chunk(WAYOBJECT_CHUNK_VERSION))
    {
    	return false;
    }

    check(F.r_chunk(WAYOBJECT_CHUNK_VERSION,&Version));

    if(Version!=WayObjectVersion)
    {
    	UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneWayPoint: Unsupported version."));
        return false;
    }

	FRBMKSceneObjectBase::LoadStream(F);

	if(!ensure(F.find_chunk(WAYOBJECT_CHUNK_POINTS)))
	{
		return false;
	}

    const uint32 WayPointsCount = F.r_u16();
    for(uint32 i = 0;i<WayPointsCount;i++)
    {
	    const TSharedPtr<FRBMKSceneWayPoint>& WayPoint = WayPoints.Add_GetRef(MakeShared<FRBMKSceneWayPoint>(TEXT("")));
		Fvector3 InPosition;
		F.r_fvector3(InPosition);
        WayPoint->Position  = FVector(StalkerMath::RBMKLocationToUnreal(InPosition));
        WayPoint->Flags = F.r_s32();
        F.r_u16();// bSelected
        shared_str InName;
        F.r_stringZ (InName);
        WayPoint->Name = ANSI_TO_TCHAR(*InName);
    }

	if(!ensure(F.find_chunk(WAYOBJECT_CHUNK_LINKS)))
    {
	    return false;
    }
    const uint32 WayLinksCount =  F.r_u16();
    for (uint32 i=0; i<WayLinksCount; i++)
    {
	    const int32 Index0 	= F.r_u16();
	    const int32 Index1 	= F.r_u16();
	    const float Probability	= F.r_float();
        WayPoints[Index0]->CreateLink(WayPoints[Index1],Probability);
    }
    return true;
}