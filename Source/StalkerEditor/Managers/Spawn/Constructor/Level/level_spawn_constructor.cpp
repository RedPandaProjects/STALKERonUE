////////////////////////////////////////////////////////////////////////////
//	Module 		: level_spawn_constructor.cpp
//	Created 	: 16.10.2004
//  Modified 	: 16.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Level spawn constructor
////////////////////////////////////////////////////////////////////////////

#include "level_spawn_constructor.h"
#include "../Game/game_spawn_constructor.h"
#include "../PatrolPaths/Storage/patrol_path_storage.h"
#include "../../Graph/Engine/graph_engine_editor.h"
#include "../../../../StalkerEditorManager.h"
#include "../../../SEFactory/StalkerSEFactoryManager.h"
#include "Resources/AIMap/StalkerAIMap.h"
#include "SpaceRestrictorWrapper/space_restrictor_wrapper.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"
THIRD_PARTY_INCLUDES_START
#include "xrServerEntities/xrMessages.h"
#include "xrServerEntities/clsid_game.h"
#include "xrServerEntities/game_base_space.h"
#include "xrServerEntities/restriction_space.h"
#include "xrEngine/object_broker.h"
#include "XrEngine/XrISEALifeSpaceRestrictor.h"
THIRD_PARTY_INCLUDES_END

#define IGNORE_ZERO_SPAWN_POSITIONS

DEFINE_LOG_CATEGORY(LogXRayLevelSpawnConstructor);
const float y_shift_correction = .15f;

CLevelSpawnConstructor::~CLevelSpawnConstructor					()
{

	for(ISE_ALifeGraphPoint* I:m_graph_points)
	{
		GStalkerEditorManager->SEFactoryManager->DestroyEntity(I->CastAbstract());
	}
	checkSlow					(!m_level_graph);
	checkSlow					(!m_cross_table);
	checkSlow					(!m_graph_engine);
}

IC	const IGameGraph &CLevelSpawnConstructor::game_graph		() const
{
	return					(m_game_spawn_constructor->game_graph());
}

IC	CLevelSpawnConstructor::LEVEL_CHANGER_STORAGE &CLevelSpawnConstructor::level_changers() const
{
	return					(m_game_spawn_constructor->level_changers());
}

IC	u32	CLevelSpawnConstructor::level_id						(shared_str level_name) const
{
	return					(m_game_spawn_constructor->level_id(*level_name));
}

IC	const ILevelGraph &CLevelSpawnConstructor::level_graph			() const
{
	return					(*m_level_graph);
}

IC	const IGameLevelCrossTable &CLevelSpawnConstructor::cross_table	() const
{
	return					(*m_cross_table);
}

IC	CGraphEngineEditor &CLevelSpawnConstructor::graph_engine			() const
{
	return					(*m_graph_engine);
}

void CLevelSpawnConstructor::init								()
{
	m_level_graph =											 LevelSpawn->AIMap;
	m_game_spawn_constructor->game_graph().set_current_level(LevelSpawn->LevelID);
	m_cross_table = &game_graph().cross_table();
	m_game_spawn_constructor->patrol_path_storage().load_editor(&level_graph(), &cross_table(), &game_graph(), LevelSpawn->Ways);
}

void CLevelSpawnConstructor::add_graph_point					(ISE_Abstract			*abstract)
{
	ISE_ALifeGraphPoint* graph_point = abstract->CastALifeGraphPoint();
	check				(graph_point);
	m_graph_points.push_back(graph_point);
}

void CLevelSpawnConstructor::add_story_object					(ISE_ALifeDynamicObject *dynamic_object)
{
	m_game_spawn_constructor->add_story_object	(dynamic_object->CastALifeObject()->m_story_id,dynamic_object);
}

void CLevelSpawnConstructor::add_space_restrictor				(ISE_ALifeDynamicObject *dynamic_object)
{
	if (!dynamic_object->CastALifeObject())
		return;
	ISE_ALifeSpaceRestrictor* space_restrictor = dynamic_object->CastALifeObject()->CastALifeSpaceRestricto();
	if (!space_restrictor)
		return;

	if (space_restrictor->m_space_restrictor_type == RestrictionSpace::eRestrictorTypeNone)
		return;

	if (!space_restrictor->CastALifeObject()->m_flags.test(ISE_ALifeObject::flCheckForSeparator))
		return;

	m_space_restrictors.push_back	(new CSpaceRestrictorWrapper(space_restrictor));
}

void CLevelSpawnConstructor::add_level_changer					(ISE_Abstract			*abstract)
{
	ISE_ALifeLevelChanger* level_changer = abstract->CastALifeObject()->CastALifeLevelChanger();
	check				(level_changer);
	m_game_spawn_constructor->add_level_changer	(level_changer);
	m_level_changers.push_back	(level_changer);
}

void CLevelSpawnConstructor::add_free_object					(ISE_Abstract			*abstract)
{
	m_game_spawn_constructor->add_object		(abstract);
}


bool CLevelSpawnConstructor::load_objects						(const TArray<FStalkerLevelSpawnData>& SpawnData)
{
	UE_LOG(LogXRayLevelSpawnConstructor, Log, TEXT("Loading spawns ..."));

	for (const FStalkerLevelSpawnData& SpawnObj : SpawnData)
	{
		NET_Packet				net_packet;
		net_packet.B.count = SpawnObj.SpawnData.Num();
		FMemory::Memcpy(net_packet.B.data, SpawnObj.SpawnData.GetData(), SpawnObj.SpawnData.Num());
		u16						ID;
		net_packet.r_begin(ID);
		check(M_SPAWN == ID);
		string64				section_name;
		net_packet.r_stringZ(section_name);
		ISE_Abstract* abstract = GStalkerEditorManager->SEFactoryManager->CreateEntity(section_name);
		if (!abstract) 
		{
			UE_LOG(LogXRayLevelSpawnConstructor,Error,TEXT("Can't create entity '%S' !"), section_name);
			continue;
		}
		abstract->Spawn_Read(net_packet);

		if (abstract->m_tClassID == CLSID_AI_GRAPH)
		{
			add_graph_point(abstract);
			continue;
		}

		ISE_ALifeObject* alife_object = abstract->CastALifeObject();
		if (!alife_object) 
		{
			GStalkerEditorManager->SEFactoryManager->DestroyEntity(abstract);
			continue;
		}

		ISE_ALifeCreatureActor* actor = alife_object->CastALifeCreatureActor();
		if (actor)
		{
			if (m_actor)
			{
				add_free_object(alife_object->CastAbstract());
				UE_LOG(LogXRayLevelSpawnConstructor, Error, TEXT("Too many actors on the level"));
				return false;
			}
			m_actor = actor;
		}

		m_spawns.push_back(alife_object);

		ISE_ALifeDynamicObject* dynamic_object = alife_object->CastALifeDynamicObject();
		if (dynamic_object) {
			add_story_object(dynamic_object);
			add_space_restrictor(dynamic_object);
		}

		if (alife_object->CastALifeLevelChanger())
			add_level_changer(abstract);
;
		add_free_object(alife_object->CastAbstract());

	}

	if (m_spawns.empty())
	{
		UE_LOG(LogXRayLevelSpawnConstructor, Error, TEXT("There are no spawn-points on the level!"));
		return false;
	}
	return true;
}

bool CLevelSpawnConstructor::correct_objects					()
{
	UE_LOG(LogXRayLevelSpawnConstructor, Log, TEXT("Correct objects ..."));
	u32						m_level_graph_vertex_id = u32(-1);
	u32						dwStart = game_graph().header().vertex_count(), dwFinish = game_graph().header().vertex_count(), dwCount = 0;
	for (u32 i=0; i<game_graph().header().vertex_count(); ++i)
		if (game_graph().vertex(i)->level_id() == LevelSpawn->LevelID) {
			if (m_level_graph_vertex_id == u32(-1))
				m_level_graph_vertex_id = i;
			dwCount++;
		}
	
	for (int i=0; i<(int)game_graph().header().vertex_count(); i++)
		if (game_graph().vertex(i)->level_id() == LevelSpawn->LevelID) {
			if (dwStart > (u32)i)
				dwStart = (u32)i;
		}
		else {
			if ((dwStart <= (u32)i) && (dwFinish > (u32)i)) {
				dwFinish = i;
				break;
			}
		}
	if (dwStart >= dwFinish) 
	{
		UE_LOG(LogXRayLevelSpawnConstructor, Error, TEXT("There are no graph vertices in the game graph for the level '%S' !"), game_graph().header().level(LevelSpawn->LevelID).name().c_str());
		return false;
	}

	for (int i=0; i<(int)m_spawns.size(); i++) 
	{
		if (!m_spawns[i]->used_ai_locations()) 
		{
			m_spawns[i]->m_tGraphID = (GameGraph::_GRAPH_ID)m_level_graph_vertex_id;
			m_spawns[i]->m_fDistance = 0.f;
			m_spawns[i]->m_tNodeID = game_graph().vertex(m_level_graph_vertex_id)->level_vertex_id();
			continue;
		}
		Fvector				position = m_spawns[i]->CastAbstract()->o_Position;
		position.y			+= y_shift_correction;
		m_spawns[i]->m_tNodeID = level_graph().vertex(u32(-1),position);
		checkSlow				(level_graph().valid_vertex_id(m_spawns[i]->m_tNodeID));
		if (m_spawns[i]->used_ai_locations() && !level_graph().inside(level_graph().vertex(m_spawns[i]->m_tNodeID),position)) 
		{
			Fvector			new_position = level_graph().vertex_position(m_spawns[i]->m_tNodeID);
#define VPUSHUE(a)	((a).x*-100), ((a).z*100), ((a).z*100)
			UE_LOG(LogXRayLevelSpawnConstructor, Warning, TEXT("[%S][%S][%S] : position changed from [%f][%f][%f] -> [%f][%f][%f]"),game_graph().header().level(LevelSpawn->LevelID).name().c_str(), *m_spawns[i]->CastAbstract()->s_name, m_spawns[i]->CastAbstract()->name_replace(), VPUSHUE(position), VPUSHUE(new_position));
#undef VPUSHUE
			m_spawns[i]->CastAbstract()->o_Position	= new_position;
		}
		u32					dwBest = cross_table().vertex(m_spawns[i]->m_tNodeID).game_vertex_id();
		if (game_graph().vertex(dwBest)->level_id() != LevelSpawn->LevelID)
		{

			UE_LOG(LogXRayLevelSpawnConstructor, Error, TEXT("Corresponding graph vertex for the spawn point is located on the ANOTHER level"));
			UE_LOG(LogXRayLevelSpawnConstructor, Error, TEXT("Current level  : [%d][%S]"), LevelSpawn->LevelID, *game_graph().header().level(LevelSpawn->LevelID).name());
			UE_LOG(LogXRayLevelSpawnConstructor, Error, TEXT("Conflict level : [%d][%S]"), game_graph().vertex(dwBest)->level_id(), *game_graph().header().level(game_graph().vertex(dwBest)->level_id()).name());
			return false;
		}

		float		fCurrentBestDistance = cross_table().vertex(m_spawns[i]->m_tNodeID).distance();
		if (dwBest == u32(-1)) 
		{
			UE_LOG(LogXRayLevelSpawnConstructor, Error, TEXT("Can't find a corresponding GRAPH VERTEX for the spawn-point %S"), m_spawns[i]->CastAbstract()->name_replace());
			UE_LOG(LogXRayLevelSpawnConstructor, Error, TEXT("Level ID    : %d"), LevelSpawn->LevelID);
			UE_LOG(LogXRayLevelSpawnConstructor, Error, TEXT("Spawn index : %d"), i);
			UE_LOG(LogXRayLevelSpawnConstructor, Error, TEXT("Spawn node  : %d"), m_spawns[i]->m_tNodeID);
			UE_LOG(LogXRayLevelSpawnConstructor, Error, TEXT("Spawn point : [%7.2f][%7.2f][%7.2f]"), -m_spawns[i]->CastAbstract()->o_Position.x*100, m_spawns[i]->CastAbstract()->o_Position.z*100, m_spawns[i]->CastAbstract()->o_Position.y*100);
	
			return false;
		}
		m_spawns[i]->m_tGraphID		= (GameGraph::_GRAPH_ID)dwBest;
		m_spawns[i]->m_fDistance	= fCurrentBestDistance;
	}
	return true;
}

void CLevelSpawnConstructor::correct_level_changers				()
{
	LEVEL_CHANGER_STORAGE::const_iterator	I = m_level_changers.begin();
	LEVEL_CHANGER_STORAGE::const_iterator	E = m_level_changers.end();
	for ( ; I != E; ++I) {
		Fvector				position = (*I)->CastAbstract()->o_Position;
		position.y			+= y_shift_correction;
		(*I)->CastALifeObject()->m_tNodeID		= level_graph().vertex(u32(-1),position);
		checkSlow				(level_graph().valid_vertex_id((*I)->CastALifeObject()->m_tNodeID));

		u32					dwBest = cross_table().vertex((*I)->CastALifeObject()->m_tNodeID).game_vertex_id();
		checkSlow				(game_graph().vertex(dwBest)->level_id() == m_level.id());
		(*I)->CastALifeObject()->m_tGraphID	= (GameGraph::_GRAPH_ID)dwBest;

		(*I)->CastALifeObject()->m_fDistance	= cross_table().vertex((*I)->CastALifeObject()->m_tNodeID).distance();
	}
}

struct remove_too_far_predicate {
	float				m_radius_sqr;
	const ILevelGraph	*m_graph;
	Fvector				m_position;

	IC			remove_too_far_predicate	(const ILevelGraph *graph, const Fvector &position, float radius)
	{
		checkSlow			(graph);
		m_graph			= graph;
		m_position		= position;
		m_radius_sqr	= _sqr(radius);
	}

	IC	bool	operator()					(const u32 &vertex_id) const
	{
		return			(m_graph->vertex_position(vertex_id).distance_to_sqr(m_position) > m_radius_sqr);
	}
};


void CLevelSpawnConstructor::generate_artefact_spawn_positions	()
{
	checkSlow(!m_graph_engine);
	m_graph_engine = new CGraphEngineEditor(m_level_graph->header().vertex_count());
	// create graph engine

	UE_LOG(LogXRayLevelSpawnConstructor,Log,TEXT("Generate artefact spawn positions ..."));
	
	FCriticalSection GenerateArtefactSpawnMutex;
	FCriticalSection GenerateArtefactSpawnLevelPointsMutex;
	ParallelFor(m_spawns.size(), [this,&GenerateArtefactSpawnMutex,&GenerateArtefactSpawnLevelPointsMutex](int32 Index)
	{

		ISE_ALifeObject* Object = m_spawns[Index];
		ISE_ALifeAnomalousZone* zone = Object->CastALifeAnomalousZone();
		if (!zone)
			return;
		ISE_Abstract* Abstract = Object->CastAbstract();
		Object->m_tNodeID = level_graph().vertex(Object->m_tNodeID, Abstract->o_Position);
		if (!level_graph().valid_vertex_position(Abstract->o_Position) || !level_graph().inside(Object->m_tNodeID, Abstract->o_Position))
			Object->m_tNodeID = level_graph().vertex(u32(-1), Abstract->o_Position);

		const IGameLevelCrossTable::CCell& cell = cross_table().vertex(Object->m_tNodeID);
		Object->m_tGraphID = cell.game_vertex_id();
		Object->m_fDistance = cell.distance();

		xr_vector<u32>						l_tpaStack;
		l_tpaStack.reserve(1024);

		{
			FScopeLock lock(&GenerateArtefactSpawnMutex);
			graph_engine().search(level_graph(),Object->m_tNodeID,Object->m_tNodeID,&l_tpaStack,SFlooder<float,u32,u32>(zone->m_offline_interactive_radius,	u32(-1),u32(-1)));
		}
		
		l_tpaStack.erase(std::remove_if(l_tpaStack.begin(),l_tpaStack.end(),remove_too_far_predicate(&level_graph(),Abstract->o_Position,zone->m_offline_interactive_radius)),l_tpaStack.end());

		if (zone->m_artefact_spawn_count >= l_tpaStack.size())
		{
			zone->m_artefact_spawn_count = l_tpaStack.size();
		}
		else
		{
			for (int32 i = l_tpaStack.size() - 1; i > 0; --i)
			{
				Swap(l_tpaStack[i], l_tpaStack[FMath::Rand() % (i + 1)]);
			}
		}
		LEVEL_POINT_STORAGE					LevelPoints;
		LevelPoints.resize(zone->m_artefact_spawn_count);

		for (int32 i=0;i< zone->m_artefact_spawn_count;i++)
		{
			LevelPoints[i].tNodeID = l_tpaStack[i];
			LevelPoints[i].tPoint = level_graph().vertex_position(l_tpaStack[i]);
			LevelPoints[i].fDistance = cross_table().vertex(l_tpaStack[i]).distance();
		}

		{
			FScopeLock lock(&GenerateArtefactSpawnLevelPointsMutex);
			zone->m_artefact_position_offset = m_level_points.size();
			m_level_points.insert(m_level_points.end(), LevelPoints.begin(), LevelPoints.end());
		}
	});
	UE_LOG(LogXRayLevelSpawnConstructor, Log, TEXT("* Completed gnerate artefact spawn positions"));
}

void CLevelSpawnConstructor::fill_level_changers				()
{
	for (u32 i=0, n=(u32)level_changers().size(); i<n; ++i) {
		if (level_id(level_changers()[i]->m_caLevelToChange) != LevelSpawn->LevelID)
			continue;

		bool found = false;
		GRAPH_POINT_STORAGE::const_iterator I = m_graph_points.begin();
		GRAPH_POINT_STORAGE::const_iterator E = m_graph_points.end();
		for ( ; I != E; ++I)
			if (!xr_strcmp(*level_changers()[i]->m_caLevelPointToChange,(*I)->CastAbstract()->name_replace())) {
				bool ok = false;
				for (u32 ii=0, nn = game_graph().header().vertex_count(); ii<nn; ++ii) 
				{
					if ((game_graph().vertex(ii)->level_id() != LevelSpawn->LevelID) || !game_graph().vertex(ii)->level_point().similar((*I)->CastAbstract()->o_Position,.001f))
						continue;
					level_changers()[i]->m_tNextGraphID		= (GameGraph::_GRAPH_ID)ii;
					level_changers()[i]->m_tNextPosition	= (*I)->CastAbstract()->o_Position;
					level_changers()[i]->m_tAngles			= (*I)->CastAbstract()->o_Angle;
					level_changers()[i]->m_dwNextNodeID		= game_graph().vertex(ii)->level_vertex_id();
					ok										= true;
					break;
				}

				check					(ok);//,"Cannot find a correspndance between graph and graph points from level editor! Rebuild graph for the level ",*level_changers()[i]->m_caLevelToChange

				level_changers().erase		(level_changers().begin() + i);
				--i;
				--n;
				found		= true;
				break;
			}

		if (!found) 
		{
			UE_LOG(LogXRayLevelSpawnConstructor, Warning, TEXT("Graph point %S not found (level changer %S)"), *level_changers()[i]->m_caLevelPointToChange, level_changers()[i]->CastAbstract()->name_replace());
		}
	}
}

void CLevelSpawnConstructor::update_artefact_spawn_positions	()
{
	u32									level_point_count = m_game_spawn_constructor->level_point_count();
	SPAWN_STORAGE::iterator				I = m_spawns.begin();
	SPAWN_STORAGE::iterator				E = m_spawns.end();
	for ( ; I != E; ++I)
	{
		ISE_Abstract					*abstract = (*I)->CastAbstract();
		ISE_ALifeObject* alife_object = abstract->CastALifeObject();
		check						(alife_object);
		checkSlow					(game_graph().vertex(alife_object->m_tGraphID)->level_id() == m_level.id());
		ISE_ALifeAnomalousZone* zone = alife_object->CastALifeAnomalousZone();
		if (zone) {
			zone->m_artefact_position_offset		= level_point_count;
			level_point_count			+= zone->m_artefact_spawn_count;
		}
	}

	m_game_spawn_constructor->add_level_points	(m_level_points);
}

bool CLevelSpawnConstructor::build							()
{
	UE_LOG(LogXRayLevelSpawnConstructor, Log,TEXT("Start build spawn in level %S[%s]"), game_graph().header().level(LevelSpawn->LevelID).name().c_str(), *LevelSpawn->Map.ToString());
	if (!load_objects(LevelSpawn->Spawns))
	{
		m_cross_table = 0;
		m_level_graph = 0;
		delete m_graph_engine;
		return false;
	}
	init								();
	
	if (!correct_objects())
	{
		m_cross_table = 0;
		m_level_graph = 0;
		delete m_graph_engine;
		return false;
	}
	generate_artefact_spawn_positions	();
	correct_level_changers				();
	if (!verify_space_restrictors())
		return false;
	
	m_cross_table						= 0;
	m_level_graph = 0;
	delete							m_graph_engine;
	UE_LOG(LogXRayLevelSpawnConstructor, Log, TEXT("Spawn build completed"));
	return true;
}

bool CLevelSpawnConstructor::update								()
{
	fill_level_changers					();
	update_artefact_spawn_positions		();
	return true;
}
bool CLevelSpawnConstructor::verify_space_restrictors			()
{
	UE_LOG(LogXRayLevelSpawnConstructor, Log, TEXT("Level [%S] : searching for AI map separators space restrictors"), game_graph().header().level(LevelSpawn->LevelID).name().c_str());
	const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>();
	if(!SGSettings->VerifySpaceRestrictorBorders)
	{
		UE_LOG(LogXRayLevelSpawnConstructor, Warning, TEXT("Skip verify space restrictors"));
		return true;
	}


	SPACE_RESTRICTORS::iterator			I = m_space_restrictors.begin();
	SPACE_RESTRICTORS::iterator			E = m_space_restrictors.end();
	bool bResult = true;
	for ( ; I != E; ++I) 
	{
		checkSlow							(*I);
		
		if ((*I)->object().m_space_restrictor_type == RestrictionSpace::eRestrictorTypeNone)
			continue;

		if (!(*I)->Verify(*m_level_graph, *m_graph_engine, m_no_separator_check))
			bResult = false;
	}

	delete_data							(m_space_restrictors);

	if (m_no_separator_check)
	{
		UE_LOG(LogXRayLevelSpawnConstructor, Log, TEXT("Level [%S] : no separators found"), game_graph().header().level(LevelSpawn->LevelID).name().c_str());
	}
	return bResult;
}
