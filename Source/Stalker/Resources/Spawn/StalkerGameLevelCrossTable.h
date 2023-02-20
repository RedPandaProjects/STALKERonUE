#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrEngine/game_level_cross_table.h"
THIRD_PARTY_INCLUDES_END
class STALKER_API StalkerGameLevelCrossTable :public IGameLevelCrossTable
{
public:
					StalkerGameLevelCrossTable	();
					~StalkerGameLevelCrossTable	();
	const CCell&	vertex						(u32 level_vertex_id) const override;
	void			Serialize					(FArchive& Ar);
	inline CHeader&	GetHeader					(){return m_tCrossTableHeader;}
	TArray<CCell>	Cells;
};