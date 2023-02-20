////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_point.h
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol point
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Resources/Spawn/StalkerLevelSpawn.h"
class CPatrolPath;
class ILevelGraph;
class IGameLevelCrossTable;
class IGameGraph;
THIRD_PARTY_INCLUDES_START
#include "xrEngine\object_interfaces.h"
#include "xrEngine\game_graph_space.h"
THIRD_PARTY_INCLUDES_END

class CPatrolPoint : public IPureSerializeObject<IReader,IWriter> {
protected:
	shared_str							m_name;
	Fvector								m_position;
	u32									m_flags;
	u32									m_level_vertex_id;
	GameGraph::_GRAPH_ID				m_game_vertex_id;
	const CPatrolPath*					m_path = nullptr;

private:
	IC		void						correct_position	(const ILevelGraph *level_graph, const IGameLevelCrossTable *cross, const IGameGraph *game_graph);

public:
										CPatrolPoint		(const ILevelGraph *level_graph, const IGameLevelCrossTable *cross, const IGameGraph *game_graph, const CPatrolPath *path, const Fvector &position, u32 level_vertex_id, u32 flags, shared_str name);
										CPatrolPoint		(const CPatrolPath *path = 0);
			void						save				(IWriter &stream) override;
			void						load				(IReader& stream) override;
			CPatrolPoint				&load_editor		(const ILevelGraph* level_graph, const IGameLevelCrossTable* cross, const IGameGraph* game_graph, const FStalkerLevelSpawnWay& way,const FStalkerLevelSpawnWayPoint& object,size_t id);
	IC		const Fvector				&position			() const;
	IC		const u32					&level_vertex_id	(const ILevelGraph *level_graph, const IGameLevelCrossTable *cross, const IGameGraph *game_graph) const;
	IC		const GameGraph::_GRAPH_ID	&game_vertex_id		(const ILevelGraph *level_graph, const IGameLevelCrossTable *cross, const IGameGraph *game_graph) const;
	IC		const u32					&flags				() const;
	IC		const shared_str			&name				() const;

};
DECLARE_LOG_CATEGORY_EXTERN(LogXRayPatrolPatthConstructor, Log, All);
#include "patrol_point_inline.h"