////////////////////////////////////////////////////////////////////////////
//	Module 		: game_spawn_constructor.cpp
//	Created 	: 16.10.2004
//  Modified 	: 16.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Game spawn constructor
////////////////////////////////////////////////////////////////////////////

#include "game_spawn_constructor.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/object_broker.h"
THIRD_PARTY_INCLUDES_END
#include "server_entity_wrapper.h"
#include "../Level/level_spawn_constructor.h"
#include "Resources/Spawn/StalkerGameSpawn.h"
#include "Resources/Spawn/StalkerLevelSpawn.h"
#include "../PatrolPaths/Storage/patrol_path_storage.h"

DEFINE_LOG_CATEGORY(LogXRayGameSpawnConstructor);
CGameSpawnConstructor::~CGameSpawnConstructor	()
{
	delete_data						(m_level_spawns);
	delete_data						(m_spawn_graph);
//	xr_delete						(m_game_graph);
	delete							m_patrol_path_storage;
}

IC	shared_str CGameSpawnConstructor::actor_level_name()
{
	string256						temp;
	return							(
		strconcat(
			sizeof(temp),
			temp,
			*game_graph().header().level(
				game_graph().vertex(
					m_actor->CastALifeObject()->m_tGraphID
				)->level_id()).name(),
			".spawn"
		)
	);
}
bool CGameSpawnConstructor::load_spawns	(UStalkerGameSpawn* GameSpawn, TArray<UStalkerLevelSpawn*>& LevelsSpawns, bool no_separator_check)
{
	m_spawn_id							= 0;
	m_spawn_graph						= new SPAWN_GRAPH;
	m_patrol_path_storage				= new CPatrolPathStorage;
	m_game_graph						= &GameSpawn->GameGraph;
	
	for (UStalkerLevelSpawn* LevelsSpawn : LevelsSpawns)
	{
		m_level_spawns.push_back(new CLevelSpawnConstructor(LevelsSpawn, this, no_separator_check));
	}

	if (m_level_spawns.empty())
	{
		UE_LOG(LogXRayGameSpawnConstructor, Error, TEXT("There are no valid levels to build spawn file from!"));
		return false;
	}
	return true;
}

bool CGameSpawnConstructor::process_spawns	()
{
	LEVEL_SPAWN_STORAGE::iterator		I = m_level_spawns.begin();
	LEVEL_SPAWN_STORAGE::iterator		E = m_level_spawns.end();
	for (CLevelSpawnConstructor*Level: m_level_spawns)
	{
		if (!Level->build())
		{
			return false;
		}
	}

	for (CLevelSpawnConstructor* Level : m_level_spawns)
	{
		if (!Level->update())
		{
			return false;
		}
	}

	if (!verify_level_changers())
		return false;
	if (!verify_spawns())
		return false;
	return true;
}

bool CGameSpawnConstructor::verify_spawns			(ALife::_SPAWN_ID spawn_id)
{
	xr_vector<ALife::_SPAWN_ID>::iterator	J = std::find(m_temp0.begin(),m_temp0.end(),spawn_id);
	if (J != m_temp0.end())
	{
		UE_LOG(LogXRayGameSpawnConstructor, Error, TEXT("RECURSIVE Spawn group chain found in spawn"));
		return false;
	}
	m_temp0.push_back						(spawn_id);

	SPAWN_GRAPH::CVertex					*vertex = m_spawn_graph->vertex(spawn_id);
	SPAWN_GRAPH::const_iterator				I = vertex->edges().begin();
	SPAWN_GRAPH::const_iterator				E = vertex->edges().end();
	for (; I != E; ++I)
		if (!verify_spawns((*I).vertex_id()))
			return false;
	return true;
}

bool CGameSpawnConstructor::verify_spawns			()
{
	SPAWN_GRAPH::const_vertex_iterator		I = m_spawn_graph->vertices().begin();
	SPAWN_GRAPH::const_vertex_iterator		E = m_spawn_graph->vertices().end();
	for ( ; I != E; ++I) {
		m_temp0.clear						();
		if (!verify_spawns((*I).second->vertex_id()))
			return false;
	}
	return true;
}
u32	CGameSpawnConstructor::level_id(LPCSTR level_name)
{
	auto	I = game_graph().header().levels().begin();
	auto	E = game_graph().header().levels().end();
	for (; I != E; ++I)
		if (!FCStringAnsi::Stricmp((*I).second.name().c_str(), level_name))
			return				((*I).second).id();
	return -1;
}
bool CGameSpawnConstructor::verify_level_changers	()
{
	if (m_level_changers.empty())
		return true;
	UE_LOG(LogXRayGameSpawnConstructor, Warning, TEXT("List of the level changers which are invalid for some reasons"));
	LEVEL_CHANGER_STORAGE::const_iterator	I = m_level_changers.begin();
	LEVEL_CHANGER_STORAGE::const_iterator	E = m_level_changers.end();
	for ( ; I != E; ++I)
	{
		UE_LOG(LogXRayGameSpawnConstructor, Warning, TEXT("\t%S"), (*I)->CastAbstract()->name_replace());
	}

	return true;
}

bool CGameSpawnConstructor::save_spawn(CMemoryWriter& stream)
{
	m_spawn_header.m_version		= XRAI_CURRENT_VERSION;
	m_spawn_header.m_guid			= game_graph().header().guid();
	m_spawn_header.m_graph_guid		= game_graph().header().guid();
	m_spawn_header.m_spawn_count	= spawn_graph().vertex_count();
	m_spawn_header.m_level_count	= (u32)m_level_spawns.size();

	stream.open_chunk(0);
	stream.w_u32(m_spawn_header.m_version);
	save_data(m_spawn_header.m_guid, stream);
	save_data(m_spawn_header.m_graph_guid, stream);
	stream.w_u32(m_spawn_header.m_spawn_count);
	stream.w_u32(m_spawn_header.m_level_count);
	stream.close_chunk();

	stream.open_chunk(1);
	save_data(spawn_graph(), stream);
	stream.close_chunk();

	stream.open_chunk(2);
	save_data(m_level_points, stream);
	stream.close_chunk();

	stream.open_chunk(3);
	save_data(m_patrol_path_storage, stream);
	stream.close_chunk();
	return true;
}

void CGameSpawnConstructor::add_story_object	(ALife::_STORY_ID id, ISE_ALifeDynamicObject *object)
{
	if (id == INVALID_STORY_ID)
		return;

	auto I = m_story_objects.find(id);
	if (I != m_story_objects.end()) 
	{
		UE_LOG					(LogXRayGameSpawnConstructor,Error, TEXT("Dublicate story id in Object1 [%S] Object2 [%S], story id %d"),object->CastAbstract()->name_replace(), (*I).second->CastAbstract()->name_replace(), object->CastALifeObject()->m_story_id);
	}
	
	m_story_objects.insert		(std::make_pair(id,object));
}

void CGameSpawnConstructor::add_object				(ISE_Abstract *object)
{
	m_critical_section.Enter	();
	object->m_tSpawnID			= spawn_id();
	spawn_graph().add_vertex	(new CServerEntityWrapper(object),object->m_tSpawnID);
	m_critical_section.Leave	();
}

void CGameSpawnConstructor::remove_object			(ISE_Abstract *object)
{
	spawn_graph().remove_vertex	(object->m_tSpawnID);
}

bool CGameSpawnConstructor::process_actor			()
{
	m_actor							= 0;
	
	LEVEL_SPAWN_STORAGE::iterator	I = m_level_spawns.begin();
	LEVEL_SPAWN_STORAGE::iterator	E = m_level_spawns.end();
	for ( ; I != E; ++I) 
	{
		if (!(*I)->actor())
			continue;
		UE_LOG(LogXRayGameSpawnConstructor, Log, TEXT("Actor is on the level %S"), *game_graph().header().level(game_graph().vertex((*I)->actor()->CastALifeObject()->m_tGraphID)->level_id()).name());

		
		if (m_actor)
		{
			UE_LOG(LogXRayGameSpawnConstructor, Error, TEXT("There must be the SINGLE level with ACTOR!"));
			return false;
		}
		
		m_actor						= (*I)->actor();
	}

	if (!m_actor)
	{
		UE_LOG(LogXRayGameSpawnConstructor, Error, TEXT("There is no ACTOR spawn point!"));
		return false;
	}

	
	return true;
}

CGameSpawnConstructor::CGameSpawnConstructor()
{
}

bool CGameSpawnConstructor::build(UStalkerGameSpawn* GameSpawn, TArray<UStalkerLevelSpawn*>& LevelsSpawn, bool no_separator_check)
{
	if (!load_spawns(GameSpawn, LevelsSpawn, no_separator_check))
		return false;
	if (!process_spawns())
		return false;
	if (!process_actor())
		return false;
	CMemoryWriter Result;
	if (!save_spawn(Result))
		return false;
	GameSpawn->SpawnData.Append(Result.pointer(), Result.size());
	return true;
}
