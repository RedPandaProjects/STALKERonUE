#include "StalkerGameGraph.h"
#include "StalkerGameSpawn.h"

StalkerGameGraph::StalkerGameGraph():LevelID(0)
{

}

StalkerGameGraph::~StalkerGameGraph()
{

}

void StalkerGameGraph::set_current_level(u32 level_id)
{
	check(CrossTables.Num() > static_cast<int32>(level_id));
	LevelID = level_id;
}

const IGameLevelCrossTable& StalkerGameGraph::cross_table() const
{
	return CrossTables[LevelID];
}

void StalkerGameGraph::Invalidate()
{
	Vertices.Empty();
	Edges.Empty();
	LevelPoints.Empty();
	CrossTables.Empty();
	FMemory::Memzero(m_header);
}

void StalkerGameGraph::Serialize(FArchive& Ar, UStalkerGameSpawn* Owner)
{
	if (Ar.IsLoading() || Ar.IsSaving())
	{
		Ar << m_header.m_version;
		Ar << m_header.m_vertex_count;
		Ar << m_header.m_edge_count;
		Ar << m_header.m_death_point_count;

		if (Ar.IsLoading())
		{
			m_header.m_levels.clear();
			FMemory::Memzero(m_header.m_guid);
			if (Owner)
			{
				BuildHeader(Owner);
			}
		}
		int32 Count = Vertices.Num();
		Ar << Count;
		if (Ar.IsLoading())
		{
			Vertices.Empty(Count);
			Vertices.AddDefaulted(Count);
		}
		for (GameGraph::CVertex& Vertex : Vertices)
		{
			Ar.Serialize(&Vertex, sizeof(IGameGraph::CVertex));
		}

		Count = Edges.Num();
		Ar << Count;
		if (Ar.IsLoading())
		{
			Edges.Empty(Count);
			Edges.AddDefaulted(Count);
		}
		for (GameGraph::CEdge& Edge : Edges)
		{
			Ar.Serialize(&Edge, sizeof(IGameGraph::CEdge));
		}

		Count = LevelPoints.Num();
		Ar << Count;
		if (Ar.IsLoading())
		{
			LevelPoints.Empty(Count);
			LevelPoints.AddDefaulted(Count);
		}
		for (GameGraph::CLevelPoint& LevelPoint : LevelPoints)
		{
			Ar.Serialize(&LevelPoint, sizeof(IGameGraph::CLevelPoint));
		}

		Count = CrossTables.Num();
		Ar << Count;
		if (Ar.IsLoading())
		{
			CrossTables.Empty(Count);
			CrossTables.AddDefaulted(Count);
		}
		for (StalkerGameLevelCrossTable& CrossTable : CrossTables)
		{
			CrossTable.Serialize(Ar);
		}
	}
}

void StalkerGameGraph::BuildHeader(class UStalkerGameSpawn* Owner)
{
	m_header.m_levels.clear();
	FMemory::Memcpy(&m_header.m_guid, &Owner->GameSpawnGuid, sizeof(FGuid));
	for (FStalkerGameSpawnLevelInfo& InLevelInfo : Owner->LevelsInfo)
	{
		IGameGraph::SLevel  LevelInfo;
		LevelInfo.m_name = TCHAR_TO_ANSI(*InLevelInfo.Name);
		LevelInfo.m_id = InLevelInfo.LeveID;
		FMemory::Memcpy(&LevelInfo.m_guid, &InLevelInfo.AIMapGuid, sizeof(FGuid));
		LevelInfo.m_offset.set(0, 0, 0);
		LevelInfo.m_section = "UwU";
		m_header.m_levels.insert(std::make_pair(LevelInfo.id(), LevelInfo));
	}
}

void StalkerGameGraph::ResetEnabled()
{
	m_enabled.assign(header().vertex_count(), true);
}

const IGameGraph::CVertex* StalkerGameGraph::get_nodes() const
{
	return Vertices.GetData();
}

const IGameGraph::CEdge* StalkerGameGraph::get_edges() const
{
	return Edges.GetData();
}

const IGameGraph::CLevelPoint* StalkerGameGraph::get_level_points() const
{
	return LevelPoints.GetData();
}

