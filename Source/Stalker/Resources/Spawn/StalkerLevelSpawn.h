#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrEngine/game_graph.h"
#include "XrEngine/game_level_cross_table.h"
THIRD_PARTY_INCLUDES_END
#include "StalkerLevelSpawn.generated.h"


USTRUCT()
struct FStalkerLevelSpawnData
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<uint8>	SpawnData;
};


USTRUCT()
struct FStalkerLevelSpawnWayPointLink
{
	GENERATED_BODY()
	UPROPERTY()
	float							Probability;
	UPROPERTY()
	int32		ToPoint;

};

USTRUCT()
struct FStalkerLevelSpawnWayPoint
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString									Name;
	UPROPERTY()
	FVector									Position;
	UPROPERTY()
	int32									Flags;
	UPROPERTY()
	TArray<FStalkerLevelSpawnWayPointLink>	Links;
};


USTRUCT()
struct FStalkerLevelSpawnWay
{
	GENERATED_BODY()
	UPROPERTY()
	FString								Name;
	UPROPERTY()
	TArray<FStalkerLevelSpawnWayPoint>	Points;
};


USTRUCT()
struct FStalkerLevelGraphPointConnection
{
	GENERATED_BODY()
	UPROPERTY()
	FString						Name;
	UPROPERTY()
	int32						VertexID;
	UPROPERTY()
	FString						ConnectionLevelName;
	UPROPERTY()
	FString						ConnectionPointName;
};



UCLASS()
class STALKER_API UStalkerLevelSpawn : public UObject
{
	GENERATED_BODY()
public:
	UStalkerLevelSpawn					();

	TArray<IGameGraph::CVertex>			GameGraphVertices;
	TArray<IGameGraph::CEdge>			GameGraphEdges;

	TArray<IGameLevelCrossTable::CCell> GameCrossTableCells;

	UPROPERTY()
	FGuid								SpawnGuid;
	UPROPERTY()
	FGuid								AIMapGuid;
	UPROPERTY()
	TArray<FStalkerLevelSpawnData>		Spawns;
	UPROPERTY()
	TArray<FStalkerLevelSpawnWay>		Ways;
	UPROPERTY()
	TMap<int32,FStalkerLevelGraphPointConnection>		GameGraphConnection;
	UPROPERTY()
	class UStalkerAIMap*				AIMap;
	UPROPERTY()
	FSoftObjectPath						Map;
	UPROPERTY(Transient)
	int32								LevelID;
	UPROPERTY()
	bool								NeedRebuild = true;
		
	void								Serialize				(FArchive& Ar) override;
	void								InvalidLevelSpawn		();
private:
	const int32							Version = 0;
};