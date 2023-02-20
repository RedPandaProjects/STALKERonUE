////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_point_inline.h
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol point inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const Fvector &CPatrolPoint::position						() const
{
	return				(m_position);
}

IC	const u32 &CPatrolPoint::flags								() const
{
	return				(m_flags);
}

IC	const shared_str &CPatrolPoint::name						() const
{
	return				(m_name);
}

IC	const u32 &CPatrolPoint::level_vertex_id					(const ILevelGraph *level_graph, const IGameLevelCrossTable *cross, const IGameGraph *game_graph) const
{
	return				(m_level_vertex_id);
}

IC	const GameGraph::_GRAPH_ID &CPatrolPoint::game_vertex_id	(const ILevelGraph *level_graph, const IGameLevelCrossTable *cross, const IGameGraph *game_graph) const
{
	return				(m_game_vertex_id);
}

