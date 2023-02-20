#include "StalkerGameLevelCrossTable.h"

StalkerGameLevelCrossTable::StalkerGameLevelCrossTable()
{

}

StalkerGameLevelCrossTable::~StalkerGameLevelCrossTable()
{

}

const IGameLevelCrossTable::CCell& StalkerGameLevelCrossTable::vertex(u32 level_vertex_id) const
{
	return Cells[level_vertex_id];
}

void StalkerGameLevelCrossTable::Serialize(FArchive& Ar)
{
	if (Ar.IsLoading() || Ar.IsSaving())
	{
		Ar.Serialize(&m_tCrossTableHeader, sizeof(IGameLevelCrossTable::CHeader));
		int32 Count = Cells.Num();
		Ar << Count;
		if (Ar.IsLoading())
		{
			Cells.Empty(Count);
			Cells.AddDefaulted(Count);
		}
		for (IGameLevelCrossTable::CCell& Cell : Cells)
		{
			Ar.Serialize(&Cell, sizeof(IGameLevelCrossTable::CCell));
		}
	}
}
