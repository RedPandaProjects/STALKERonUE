#pragma once

class FRBMKSceneWayPoint;

struct FRBMKSceneWayPointLink
{
	FRBMKSceneWayPointLink(TWeakPtr<FRBMKSceneWayPoint> InWayPoint,float InProbability):WayPoint(InWayPoint),Probability(InProbability){}
	inline bool operator==(const FRBMKSceneWayPointLink&Right) const
	{
		return WayPoint == Right.WayPoint&&Probability == Right.Probability;
	}
	TWeakPtr<FRBMKSceneWayPoint>	WayPoint;
    float							Probability;
	
};

class FRBMKSceneWayPoint
{

	
public:
									FRBMKSceneWayPoint		(const FString&InName);
									~FRBMKSceneWayPoint		();
    void							CreateLink				(TWeakPtr<FRBMKSceneWayPoint> Point, float Probability);
    bool							AppendLink				(TWeakPtr<FRBMKSceneWayPoint> Point, float Probability);
    bool							DeleteLink				(TWeakPtr<FRBMKSceneWayPoint> Point);
	FRBMKSceneWayPointLink*			FindLink				(TWeakPtr<FRBMKSceneWayPoint> Point);
	FString							Name;
	FVector							Position;
	TArray<FRBMKSceneWayPointLink>	Links;
	int32							Flags;
};

class FRBMKSceneWayObject: public FRBMKSceneObjectBase
{
public:
													FRBMKSceneWayObject		(const FString&Name);
													~FRBMKSceneWayObject	() override;

  	bool											LoadStream				(IReader&) override;
  	bool											LoadLTX					(CInifile& INIFile, const FString&SectionName) override;

	void*											QueryInterface			(ERBMKSceneObjectType InClassID) override;

	TArray<TSharedPtr<FRBMKSceneWayPoint>>			WayPoints;
private:
	static constexpr uint32 WayObjectVersion = 0x0013;
};