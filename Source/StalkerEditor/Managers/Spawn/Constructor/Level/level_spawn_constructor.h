////////////////////////////////////////////////////////////////////////////
//	Module 		: level_spawn_constructor.h
//	Created 	: 16.10.2004
//  Modified 	: 16.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Level spawn constructor
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../spawn_constructor_space.h"
#include "Resources/Spawn/StalkerLevelSpawn.h"

class ILevelGraph;
class IGameLevelCrossTable;
class CGameSpawnConstructor;
class ISE_ALifeCreatureActor;
class CGraphEngineEditor;
class ISE_Abstract;
class ISE_ALifeObject;
class ISE_ALifeGraphPoint;
//class ISE_SpawnGroup;
class ISE_ALifeAnomalousZone;
class CSpaceRestrictorWrapper;
class CPatrolPathStorage;
class ISE_ALifeDynamicObject;

class CLevelSpawnConstructor  {
public:
	typedef SpawnConstructorSpace::LEVEL_POINT_STORAGE			LEVEL_POINT_STORAGE;
	typedef xr_vector<ISE_ALifeLevelChanger*>					LEVEL_CHANGER_STORAGE;
	typedef xr_vector<ISE_ALifeObject*>							SPAWN_STORAGE;
	typedef xr_vector<ISE_ALifeGraphPoint*>						GRAPH_POINT_STORAGE;
	typedef xr_vector<CSpaceRestrictorWrapper*>					SPACE_RESTRICTORS;
//	typedef xr_vector<ISE_Abstract*>							GROUP_OBJECTS;
//	typedef xr_map<shared_str,GROUP_OBJECTS*>					SPAWN_GRPOUP_OBJECTS;
//	typedef xr_map<shared_str,ISE_SpawnGroup*>					SPAWN_GROUPS;

private:
	SPAWN_STORAGE						m_spawns;
	LEVEL_POINT_STORAGE					m_level_points;
	GRAPH_POINT_STORAGE					m_graph_points;
	SPACE_RESTRICTORS					m_space_restrictors;
//	SPAWN_GRPOUP_OBJECTS				m_spawn_objects;
//	SPAWN_GROUPS						m_spawn_groups;
	CGameSpawnConstructor				*m_game_spawn_constructor;
	ISE_ALifeCreatureActor				*m_actor;
	ILevelGraph							*m_level_graph;
	CGraphEngineEditor					*m_graph_engine;
	LEVEL_CHANGER_STORAGE				m_level_changers;
	bool								m_no_separator_check;

private:
	const class UStalkerLevelSpawn*		LevelSpawn = nullptr;
	const IGameLevelCrossTable			*m_cross_table;

protected:
			void						init								();
			bool						load_objects						(const TArray<FStalkerLevelSpawnData>& SpawnData);
			bool						correct_objects						();
			void						generate_artefact_spawn_positions	();
			void						correct_level_changers				();
			bool						verify_space_restrictors			();
			void						fill_level_changers					();
			void						add_graph_point						(ISE_Abstract			*abstract);
			void						add_story_object					(ISE_ALifeDynamicObject *dynamic_object);
			void						add_space_restrictor				(ISE_ALifeDynamicObject *dynamic_object);
			void						add_free_object						(ISE_Abstract			*abstract);
			void						add_level_changer					(ISE_Abstract			*abstract);
			void						update_artefact_spawn_positions		();
	IC		const IGameGraph			&game_graph							() const;
	IC		const ILevelGraph			&level_graph						() const;
	IC		const IGameLevelCrossTable	&cross_table						() const;
	IC		CGraphEngineEditor			&graph_engine						() const;
	IC		LEVEL_CHANGER_STORAGE		&level_changers						() const;
	IC		u32							level_id							(shared_str level_name) const;

public:
	IC									CLevelSpawnConstructor				(class UStalkerLevelSpawn* LevelSpawn, CGameSpawnConstructor *game_spawn_constructor, bool no_separator_check);
	virtual								~CLevelSpawnConstructor				();
	virtual bool						build								();
	IC		ISE_ALifeCreatureActor		*actor								() const;
	IC		const IGameGraph::SLevel	&level								() const;
			bool						update								();
	IC		CGameSpawnConstructor		&game_spawn_constructor				() const;
};
DECLARE_LOG_CATEGORY_EXTERN(LogXRayLevelSpawnConstructor, Log, All);
#include "level_spawn_constructor_inline.h"