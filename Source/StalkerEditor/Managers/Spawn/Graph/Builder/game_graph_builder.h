////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_builder.h
//	Created 	: 14.12.2005
//  Modified 	: 14.12.2005
//	Author		: Dmitriy Iassenev
//	Description : Game graph builder
////////////////////////////////////////////////////////////////////////////

#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrEngine/alife_space.h"
#include "XrEngine/game_graph.h"
#include "XrEngine/graph_vertex.h"
#include "XrEngine/graph_edge.h"
#include "XrEngine/graph_abstract.h"
THIRD_PARTY_INCLUDES_END

class ILevelGraph;
class IGameLevelCrossTable;
class CGraphEngineEditor;

class CGameGraphBuilder 
{
private:
	typedef GameGraph::CVertex						vertex_type;
	typedef CGraphAbstract<vertex_type,float,u32>	graph_type;
	typedef xr_vector<xr_vector<u32> >				DISTANCES;
	typedef std::pair<u32,u32>						PAIR;
	typedef std::pair<float,PAIR>					TRIPPLE;
	typedef xr_vector<TRIPPLE>						TRIPPLES;

private:
	LPCSTR							m_graph_name;
	LPCSTR							m_cross_table_name;

private:
	shared_str						m_level_name;

private:
	ILevelGraph			*			m_level_graph;
	graph_type			*			m_graph;
	// cross table generation stuff
	xr_vector<bool>					m_marks;
	xr_vector<u32>					m_mark_stack;
	DISTANCES						m_distances;
	xr_vector<u32>					m_current_fringe;
	xr_vector<u32>					m_next_fringe;
	xr_vector<u32>					m_results;
	// cross table itself
	TRIPPLES						m_tripples;
	xr_vector<u32>					m_path;
	CGraphEngineEditor	*			m_graph_engine;

private:

private:
			void					mark_vertices		(u32 level_vertex_id);
			void					fill_marks			();
			void					fill_distances		();
			void					recursive_update	(const u32 &index);
			void					iterate_distances	();
			void					build_cross_table	(class UStalkerLevelSpawn* LevelSpawn);
	
private:
			void					fill_neighbours		(class UStalkerLevelSpawn* LevelSpawn,const u32 &game_vertex_id);
			float					path_distance		(const u32 &game_vertex_id0, const u32 &game_vertex_id1);
			void					generate_edges		(const u32 &vertex_id);
			void					generate_edges		(class UStalkerLevelSpawn* LevelSpawn);
			void					connectivity_check	();
			void					create_tripples		();
			void					process_tripple		(const TRIPPLE &tripple);
			void					optimize_graph		();
			void					build_game_graph	(class UStalkerLevelSpawn* LevelSpawn);

public:

	IC		ILevelGraph&			level_graph			() const;
	IC		graph_type&				graph				() const;

									CGameGraphBuilder	(ILevelGraph*LevelGraph);
									~CGameGraphBuilder	();
			void					load_graph_point	(class UStalkerLevelSpawn* LevelSpawn,ISE_Abstract* entity);
			bool					build_graph			(class UStalkerLevelSpawn*LevelSpawn);
};
DECLARE_LOG_CATEGORY_EXTERN(LogXRayGameGraphBuilder, Log, All);
#include "game_graph_builder_inline.h"