#include "StalkerGameSpawn.h"


void UStalkerGameSpawn::InvalidGameSpawn()
{
	GameGraph.Invalidate();
	GameSpawnGuid.Invalidate();
	LevelsInfo.Empty();
	SpawnData.Empty();
	Modify();
}

void UStalkerGameSpawn::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (Ar.IsLoading() || Ar.IsSaving())
	{
		int32 CurrentVersion = Version;
		Ar << CurrentVersion;
		check(CurrentVersion == 0);
		Ar<< SpawnData;
	}
	GameGraph.Serialize(Ar,this);
}
