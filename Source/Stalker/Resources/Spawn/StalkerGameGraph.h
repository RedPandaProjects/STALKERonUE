#pragma once
#include "StalkerGameLevelCrossTable.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/game_graph.h"
THIRD_PARTY_INCLUDES_END
class STALKER_API StalkerGameGraph:public IGameGraph
{

public:
										StalkerGameGraph		();
										~StalkerGameGraph		();

	void								set_current_level		(u32 level_id) override;
	const IGameLevelCrossTable&			cross_table				() const override;
	void								Invalidate				();
	void								Serialize				(FArchive& Ar,class UStalkerGameSpawn*Owner);
	void								BuildHeader				(class UStalkerGameSpawn* Owner);
	inline CHeader&						GetHeader				(){return m_header;}
	void								ResetEnabled			();
	TArray<CVertex>						Vertices;
	TArray<CEdge>						Edges;
	TArray<CLevelPoint>					LevelPoints;
	int32								LevelID;
	TArray<StalkerGameLevelCrossTable>	CrossTables;
protected:
	const CVertex*						get_nodes				() const override;
	const CEdge*						get_edges				() const override;
	const CLevelPoint*					get_level_points		() const override;


};