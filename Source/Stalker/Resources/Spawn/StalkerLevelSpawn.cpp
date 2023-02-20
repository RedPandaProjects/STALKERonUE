#include "StalkerLevelSpawn.h"

UStalkerLevelSpawn::UStalkerLevelSpawn	()
{
	InvalidLevelSpawn();
}

void UStalkerLevelSpawn::Serialize		(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (Ar.IsLoading() || Ar.IsSaving())
	{
		int32 CurrentVersion = Version;
		Ar << CurrentVersion;
		check(CurrentVersion==0);
		{
			int32 Count = GameGraphVertices.Num();
			Ar << Count;
			if (Ar.IsLoading())
			{
				GameGraphVertices.Empty(Count);
				GameGraphVertices.AddDefaulted(Count);
			}
			for (GameGraph::CVertex&Vertex: GameGraphVertices)
			{
				Ar.Serialize(&Vertex, sizeof(IGameGraph::CVertex));
			}

			Count = GameGraphEdges.Num();
			Ar << Count;
			if (Ar.IsLoading())
			{
				GameGraphEdges.Empty(Count);
				GameGraphEdges.AddDefaulted(Count);
			}
			for (GameGraph::CEdge& Edge : GameGraphEdges)
			{
				Ar.Serialize(&Edge, sizeof(IGameGraph::CEdge));
			}
		}
		{
			int32 Count = GameCrossTableCells.Num();
			Ar << Count;
			if (Ar.IsLoading())
			{
				GameCrossTableCells.Empty(Count);
				GameCrossTableCells.AddDefaulted(Count);
			}
			for (IGameLevelCrossTable::CCell& Cell : GameCrossTableCells)
			{
				Ar.Serialize(&Cell, sizeof(IGameLevelCrossTable::CCell));
			}
		}
	}
}

void UStalkerLevelSpawn::InvalidLevelSpawn	()
{
	GameGraphConnection.Empty();
	SpawnGuid.Invalidate();
	AIMapGuid.Invalidate();
	Map.Reset();
	Ways.Empty();
	GameGraphVertices.Empty();
	GameGraphEdges.Empty();
	GameCrossTableCells.Empty();
	Spawns.Empty();
	AIMap = nullptr;
	LevelID = -1;
	NeedRebuild = true;
	Modify();
}
