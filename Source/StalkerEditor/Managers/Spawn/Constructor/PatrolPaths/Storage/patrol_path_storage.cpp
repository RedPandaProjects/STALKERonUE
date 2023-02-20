////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_storage.cpp
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path storage
////////////////////////////////////////////////////////////////////////////

#include "patrol_path_storage.h"
#include "../Path/patrol_path.h"
#include "../Point/patrol_point.h"

DEFINE_LOG_CATEGORY(LogXRayPatrolPatthConstructor);
CPatrolPathStorage::~CPatrolPathStorage		()
{
	delete_data					(m_registry);
}

void CPatrolPathStorage::load_editor(const ILevelGraph* level_graph, const IGameLevelCrossTable* cross, const IGameGraph* game_graph, const TArray<FStalkerLevelSpawnWay>& Ways)
{
	for (const FStalkerLevelSpawnWay& Way : Ways)
	{
		shared_str	patrol_name = TCHAR_TO_ANSI(*Way.Name);
		const_iterator			I = m_registry.find(patrol_name);
		if (I != m_registry.end())
		{
			UE_LOG(LogXRayPatrolPatthConstructor,Error,TEXT("Duplicated patrol path found %s"), *Way.Name);
		}
		else
		{
			CPatrolPath* PatrolPath=	new CPatrolPath(patrol_name);
			PatrolPath->load_editor(level_graph,cross,game_graph,Way);
			m_registry.insert(std::make_pair(patrol_name,PatrolPath));
		}
	}
}
void CPatrolPathStorage::load(IReader& stream)
{
	check(false);
}
void CPatrolPathStorage::save				(IWriter &stream)
{
	stream.open_chunk			(0);
	stream.w_u32				(m_registry.size());
	stream.close_chunk			();

	stream.open_chunk			(1);

	PATROL_REGISTRY::iterator	I = m_registry.begin();
	PATROL_REGISTRY::iterator	E = m_registry.end();
	for (int i=0; I != E; ++I, ++i) {
		stream.open_chunk		(i);

		stream.open_chunk		(0);
        save_data				((*I).first,stream);
		stream.close_chunk		();

		stream.open_chunk		(1);
        save_data				((*I).second,stream);
		stream.close_chunk		();

		stream.close_chunk		();
	}

	stream.close_chunk			();
}
