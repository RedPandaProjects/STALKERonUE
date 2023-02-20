#pragma once
#include "StalkerGameGraph.h"
#include "StalkerGameSpawn.generated.h"

USTRUCT()
struct FStalkerGameSpawnLevelInfo
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	FString			Name;

	UPROPERTY()
	FSoftObjectPath Map;
	
	UPROPERTY()
	int32			LeveID;

	UPROPERTY()
	FGuid			AIMapGuid;

	UPROPERTY()
	FGuid			LevelSpawnGuid;
};

UCLASS()
class STALKER_API UStalkerGameSpawn : public UObject
{
	GENERATED_BODY()
public:
	void								InvalidGameSpawn	();
	void								Serialize			(FArchive& Ar) override;

	UPROPERTY()
	FGuid								GameSpawnGuid;

	UPROPERTY()
	TArray<FStalkerGameSpawnLevelInfo>	LevelsInfo;

	StalkerGameGraph					GameGraph;

	TArray<uint8>						SpawnData;
private:
	const int32							Version = 0;
};