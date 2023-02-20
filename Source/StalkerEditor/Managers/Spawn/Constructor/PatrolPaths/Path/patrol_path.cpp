////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path.cpp
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path
////////////////////////////////////////////////////////////////////////////

#include "patrol_path.h"
#include "../Point/patrol_point.h"

CPatrolPath::CPatrolPath			(shared_str name)
{
#ifdef DEBUG
	m_name			= name;
#endif
}

CPatrolPath::~CPatrolPath	()
{
}

CPatrolPath& CPatrolPath::load_editor(const ILevelGraph* level_graph, const IGameLevelCrossTable* cross, const IGameGraph* game_graph,const FStalkerLevelSpawnWay& object)
{
	u32				vertex_count = object.Points.Num();
	int l_cnt = 0;
	for (u32 i = 0; i < vertex_count; ++i)
	{
		add_vertex(CPatrolPoint(this).load_editor(level_graph, cross, game_graph, object, object.Points[i], i), i);
	}

	for(int32 i=0;i < object.Points.Num();i++)
	{
		for (const FStalkerLevelSpawnWayPointLink&Link: object.Points[i].Links)
		{
			u16			vertex0 = static_cast<u16>(i);
			u16			vertex1 = static_cast<u16>(Link.ToPoint);
			float		probability = Link.Probability;
			add_edge(vertex0, vertex1, probability);
		}
	}
	return			(*this);
}

#ifdef DEBUG
void CPatrolPath::load		(IReader &stream)
{
	inherited::load	(stream);
	vertex_iterator	I = vertices().begin();
	vertex_iterator	E = vertices().end();
	for ( ; I != E; ++I)
		(*I).second->data().path	(this);
}
#endif
