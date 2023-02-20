////////////////////////////////////////////////////////////////////////////
//	Module 		: game_spawn_constructor.h
//	Created 	: 16.10.2004
//  Modified 	: 16.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Game spawn constructor
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "server_entity_wrapper.h"
#include "../spawn_constructor_space.h"

class ISE_Abstract;
class CLevelSpawnConstructor;
class ISE_ALifeCreatureAbstract;
class CPatrolPathStorage;

struct CLevelInfo {
	u8				m_id;
	shared_str		m_name;
	Fvector			m_offset;
	shared_str		m_section;

	CLevelInfo(u8 id, shared_str name, const Fvector& offset, shared_str section) :
		m_id(id),
		m_name(name),
		m_offset(offset),
		m_section(section)
	{
	}

	IC	bool	operator< (const CLevelInfo& info) const
	{
		return		(m_id < info.m_id);
	}
};

class CGameSpawnConstructor 
{
	friend class CSpawnMerger;
public:
	typedef SpawnConstructorSpace::LEVEL_POINT_STORAGE								LEVEL_POINT_STORAGE;
	typedef xr_vector<ISE_ALifeLevelChanger*>							LEVEL_CHANGER_STORAGE;
	typedef CGraphAbstractSerialize<CServerEntityWrapper*,float,ALife::_SPAWN_ID>	SPAWN_GRAPH;
	typedef xr_vector<CLevelSpawnConstructor*>										LEVEL_SPAWN_STORAGE;
	typedef xr_set<CLevelInfo>														LEVEL_INFO_STORAGE;

public:
	struct CSpawnHeader {
		u32							m_version;
		xrGUID						m_guid;
		xrGUID						m_graph_guid;
		u32							m_spawn_count;
		u32							m_level_count;
	};

private:
	xrCriticalSection				m_critical_section;
	ALife::_SPAWN_ID				m_spawn_id;
	CSpawnHeader					m_spawn_header;
	xr_map<ALife::_STORY_ID, ISE_ALifeDynamicObject*> m_story_objects;
	LEVEL_SPAWN_STORAGE				m_level_spawns;
	LEVEL_CHANGER_STORAGE			m_level_changers;
	LEVEL_POINT_STORAGE				m_level_points;
	bool							m_no_separator_check;

private:
	xr_vector<ALife::_SPAWN_ID>		m_spawn_roots;
	xr_vector<ALife::_SPAWN_ID>		m_temp0;
	xr_vector<ALife::_SPAWN_ID>		m_temp1;

private:
	IGameGraph						*m_game_graph;
	SPAWN_GRAPH						*m_spawn_graph;
	CPatrolPathStorage				*m_patrol_path_storage;
	//CInifile						*m_game_info;
	ISE_ALifeCreatureActor *m_actor;

private:
	string_path						m_game_graph_id;

private:
	IC		shared_str				actor_level_name		();
			bool					save_spawn				( CMemoryWriter& output);
			bool					verify_level_changers	();
			bool					verify_spawns			(ALife::_SPAWN_ID spawn_id);
			bool					verify_spawns			();
			bool					process_spawns			();
			bool					load_spawns				(class UStalkerGameSpawn* GameSpawn, TArray<class UStalkerLevelSpawn*>& LevelsSpawn, bool no_separator_check);
	IC		SPAWN_GRAPH				&spawn_graph			();
	IC		ALife::_SPAWN_ID		spawn_id				();
	IC		void					process_spawns			(xr_vector<ALife::_SPAWN_ID> &spawns);
			bool					process_actor			();

public:
									CGameSpawnConstructor	();
			bool					 build					(class UStalkerGameSpawn*GameSpawn,TArray<class UStalkerLevelSpawn*>&LevelsSpawn, bool no_separator_check = true);
	virtual							~CGameSpawnConstructor	();
			void					add_story_object		(ALife::_STORY_ID id,ISE_ALifeDynamicObject *object);
			void					add_object				(ISE_Abstract *object);
			void					remove_object			(ISE_Abstract *object);
	IC		void					add_level_changer		(ISE_ALifeLevelChanger *level_changer);
	IC		void					add_level_points		(const LEVEL_POINT_STORAGE &level_points);
			u32						level_id				(LPCSTR level_name);
	IC		IGameGraph				&game_graph				() const;
//	IC		CInifile				&game_info				();
	IC		void					add_edge				(ALife::_SPAWN_ID id0, ALife::_SPAWN_ID id1, float weight);
	IC		u32						level_point_count		() const;
	IC		LEVEL_CHANGER_STORAGE	&level_changers			();
	IC		CPatrolPathStorage		&patrol_path_storage	() const;
};

DECLARE_LOG_CATEGORY_EXTERN(LogXRayGameSpawnConstructor, Log, All);
#include "game_spawn_constructor_inline.h"