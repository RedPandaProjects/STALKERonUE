////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_point.cpp
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol point
////////////////////////////////////////////////////////////////////////////

#include "patrol_point.h"
THIRD_PARTY_INCLUDES_START
#include "xrEngine/object_broker.h"
#include "xrServerEntities/ai_space.h"
THIRD_PARTY_INCLUDES_END

#	include "../Path/patrol_path.h"

CPatrolPoint::CPatrolPoint									(const CPatrolPath *path)
{
	m_path = path;
}


void CPatrolPoint::load(IReader& stream)
{
	check(false);
}
IC	void CPatrolPoint::correct_position						(const ILevelGraph *level_graph, const IGameLevelCrossTable *cross, const IGameGraph *game_graph)
{
	if (!level_graph || !level_graph->valid_vertex_position(position()) || !level_graph->valid_vertex_id(m_level_vertex_id))
		return;

	if (!level_graph->inside(level_vertex_id(level_graph,cross,game_graph),position()))
		m_position		= level_graph->vertex_position(level_vertex_id(level_graph,cross,game_graph));

	m_game_vertex_id	= cross->vertex(level_vertex_id(level_graph,cross,game_graph)).game_vertex_id();
}

CPatrolPoint::CPatrolPoint									(const ILevelGraph *level_graph, const IGameLevelCrossTable *cross, const IGameGraph *game_graph, const CPatrolPath *path, const Fvector &position, u32 level_vertex_id, u32 flags, shared_str name)
{
	m_position			= position;
	m_level_vertex_id	= level_vertex_id;
	m_flags				= flags;
	m_name				= name;
	correct_position	(level_graph,cross,game_graph);
}


CPatrolPoint& CPatrolPoint::load_editor(const ILevelGraph* level_graph, const IGameLevelCrossTable* cross, const IGameGraph* game_graph, const FStalkerLevelSpawnWay& way,const FStalkerLevelSpawnWayPoint& object, size_t id)
{
	m_position = StalkerMath::UnrealLocationToRBMK(object.Position);
	m_flags = object.Flags;
	m_name = TCHAR_TO_ANSI(*object.Name);
	m_level_vertex_id = 0xFFFFFFFF;
	if (level_graph && level_graph->valid_vertex_position(m_position)) 
	{

		Fvector				position = m_position;
		position.y += .15f;
		m_level_vertex_id = level_graph->vertex_id(position);
	}

	correct_position(level_graph, cross, game_graph);
	return *this;
}


void CPatrolPoint::save										(IWriter &stream)
{
	save_data			(m_name,stream);
	save_data			(m_position,stream);
	save_data			(m_flags,stream);
	save_data			(m_level_vertex_id,stream);
	save_data			(m_game_vertex_id,stream);
}
