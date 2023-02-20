////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_builder.cpp
//	Created 	: 14.12.2005
//  Modified 	: 14.12.2005
//	Author		: Dmitriy Iassenev
//	Description : Game graph builder
////////////////////////////////////////////////////////////////////////////

#include "game_graph_builder.h"
#include "../Engine/graph_engine_editor.h"
#include "../Engine/graph_engine_editor_space.h"
#include "../../PathManager/Params/path_manager_params.h"
#include "../../PathManager/Params/path_manager_params_straight_line.h"
#include "Resources/Spawn/StalkerLevelSpawn.h"


DEFINE_LOG_CATEGORY(LogXRayGameGraphBuilder);
CGameGraphBuilder::CGameGraphBuilder		(ILevelGraph* LevelGraph)
{
	m_graph					= new graph_type();
	m_level_graph           = LevelGraph;
}

CGameGraphBuilder::~CGameGraphBuilder		()
{
	delete m_graph;
}


void CGameGraphBuilder::load_graph_point	(class UStalkerLevelSpawn* LevelSpawn, ISE_Abstract*entity)
{


	ISE_ALifeGraphPoint* graph_point = entity->CastALifeGraphPoint();
	if (!graph_point) {
		return;
	}
	vertex_type				vertex;
	vertex.tLocalPoint		= entity->o_Position;
	// check for duplicate graph point positions
	{
		graph_type::const_vertex_iterator	I = graph().vertices().begin();
		graph_type::const_vertex_iterator	E = graph().vertices().end();
		for ( ; I != E; ++I) {
			if ((*I).second->data().tLocalPoint.distance_to_sqr(vertex.tLocalPoint) < EPS_L) {
				UE_LOG(LogXRayGameGraphBuilder, Warning, TEXT("! removing graph point [%s][%f][%f][%f] because it is too close to the another graph point" ), ANSI_TO_TCHAR(entity->name_replace()),VPUSH(entity->o_Position));
			
				return;
			}
		}
	}

	vertex.tGlobalPoint		= entity->o_Position;
	vertex.tNodeID			= level_graph().valid_vertex_position(vertex.tLocalPoint) ? level_graph().vertex_id(vertex.tLocalPoint) : u32(-1);
	if (!level_graph().valid_vertex_id(vertex.tNodeID)) {
		UE_LOG(LogXRayGameGraphBuilder, Warning, TEXT("removing graph point [%s][%f][%f][%f] because it is outside of the AI map"), ANSI_TO_TCHAR(entity->name_replace()),VPUSH(entity->o_Position));
		
		return;
	}

	{
		graph_type::const_vertex_iterator	I = graph().vertices().begin();
		graph_type::const_vertex_iterator	E = graph().vertices().end();
		for ( ; I != E; ++I) {
			if ((*I).second->data().tNodeID == vertex.tNodeID) {
				UE_LOG(LogXRayGameGraphBuilder, Warning, TEXT("removing graph point [%s][%f][%f][%f] because it has the same AI node as another graph point"),ANSI_TO_TCHAR(entity->name_replace()),VPUSH(entity->o_Position));
			
				return;
			}
		}
	}

	vertex.tNeighbourCount	= 0;
	FMemory::Memcpy			(vertex.tVertexTypes,graph_point->m_tLocations,GameGraph::LOCATION_TYPE_COUNT*sizeof(GameGraph::_LOCATION_ID));
	vertex.tLevelID			= 0;
	vertex.tDeathPointCount = 0;
	vertex.dwPointOffset	= 0;
	FStalkerLevelGraphPointConnection&Conection = 	LevelSpawn->GameGraphConnection.Add(graph().vertices().size());
	Conection.VertexID = graph().vertices().size();
	Conection.Name = entity->name_replace();
	Conection.ConnectionLevelName = graph_point->m_caConnectionLevelName.c_str();
	Conection.ConnectionPointName = graph_point->m_caConnectionPointName.c_str();
	graph().add_vertex		(vertex,graph().vertices().size());
}


template <typename T>
IC	bool sort_predicate_less(const T &first, const T &second)
{
	return					(first.first < second.first);
}

template <typename T>
IC	bool sort_predicate_greater(const T &first, const T &second)
{
	return					(first.first > second.first);
}

void CGameGraphBuilder::mark_vertices		(u32 level_vertex_id)
{
	ILevelGraph::const_iterator			I, E;
	m_mark_stack.reserve				(8192);
	m_mark_stack.push_back				(level_vertex_id);

	for ( ; !m_mark_stack.empty(); ) {
		level_vertex_id					= m_mark_stack.back();
		m_mark_stack.resize				(m_mark_stack.size() - 1);
		const ILevelGraph::CVertex			*node = level_graph().vertex(level_vertex_id);
		level_graph().begin				(level_vertex_id,I,E);
		m_marks[level_vertex_id]		= true;
		for ( ; I != E; ++I) {
			u32							next_level_vertex_id = node->link(I);
			if (!level_graph().valid_vertex_id(next_level_vertex_id))
				continue;
			
			if (!m_marks[next_level_vertex_id])
				m_mark_stack.push_back	(next_level_vertex_id);
		}
	}
}

void CGameGraphBuilder::fill_marks			()
{

	m_marks.assign						(level_graph().header().vertex_count(),false);
	graph_type::const_vertex_iterator	I = graph().vertices().begin();
	graph_type::const_vertex_iterator	E = graph().vertices().end();
	for ( ; I != E; ++I)
		mark_vertices					((*I).second->data().level_vertex_id());
	m_marks.flip						();
	
}

void CGameGraphBuilder::fill_distances		()
{

	m_distances.resize					(graph().vertices().size());
	{
		DISTANCES::iterator				I = m_distances.begin();
		DISTANCES::iterator				E = m_distances.end();
		for ( ; I != E; I++) {
			(*I).resize					(level_graph().header().vertex_count());
			xr_vector<u32>::iterator	i = (*I).begin();
			xr_vector<u32>::iterator	e = (*I).end();
			for ( ; i != e; i++)
				*i						= u32(-1);
		}
	}
}

void CGameGraphBuilder::recursive_update	(const u32 &game_vertex_id)
{

	u32							level_vertex_id = graph().vertex(game_vertex_id)->data().level_vertex_id();
	xr_vector<u32>				&distances = m_distances[game_vertex_id];
	m_distances[m_results[level_vertex_id]][level_vertex_id]	= u32(-1);

	m_current_fringe.reserve	(distances.size());
	m_next_fringe.reserve		(distances.size());
	distances.assign			(distances.size(),u32(-1));
	m_current_fringe.push_back	(level_vertex_id);
	u32							curr_dist = 0;
	u32							total_count = 0;

	u32							vertex_count = graph().header().vertex_count();


	for ( ; !m_current_fringe.empty(); ) {
		xr_vector<u32>::iterator			I = m_current_fringe.begin();
		xr_vector<u32>::iterator			E = m_current_fringe.end();
		for ( ; I != E; ++I) {
			u32								*result = &m_results[*I];
			check							(curr_dist < m_distances[*result][*I]);
			*result							= game_vertex_id;

			distances[*I]					= curr_dist;
			ILevelGraph::const_iterator		i, e;
			const ILevelGraph::CVertex			*node = level_graph().vertex(*I);
			level_graph().begin				(*I,i,e);
			for ( ; i != e; ++i) {
				u32							dwNexNodeID = node->link(i);
				if (!level_graph().valid_vertex_id(dwNexNodeID))
					continue;
				
				if (m_marks[dwNexNodeID])
					continue;
				
				if (distances[dwNexNodeID] <= curr_dist)
					continue;

				if (m_distances[m_results[dwNexNodeID]][dwNexNodeID] <= (curr_dist + 1))
					continue;

				m_next_fringe.push_back		(dwNexNodeID);
				m_marks[dwNexNodeID]		= true;
			}
		}
		
		I = m_current_fringe.begin();
		E = m_current_fringe.end();
		for ( ; I != E; ++I)
			m_marks[*I]		= false;

		total_count			+= m_current_fringe.size();
		m_current_fringe	= m_next_fringe;
		m_next_fringe.clear	();
		++curr_dist;

	}

}

void CGameGraphBuilder::iterate_distances	()
{

	m_results.assign					(level_graph().header().vertex_count(),0);
	
	for (int i=0, n=(int)graph().vertices().size(); i<n; ++i) {
		if (i) {
			for (int k=0, kn=(int)level_graph().header().vertex_count(); k<kn; ++k)
				m_distances[i][k]		= m_distances[i - 1][k];
		}

		recursive_update				(i);
	}

}



void CGameGraphBuilder::build_cross_table	(class UStalkerLevelSpawn* LevelSpawn)
{
	UE_LOG(LogXRayGameGraphBuilder, Log, TEXT("Building cross table"));

	fill_marks				();
	fill_distances			();
	iterate_distances		();

	//IGameLevelCrossTable::CHeader&tCrossTableHeader = LevelSpawn->GameCrossTableHeader;
	//tCrossTableHeader.dwVersion = XRAI_CURRENT_VERSION;
	//tCrossTableHeader.dwNodeCount = level_graph().header().vertex_count();
	//tCrossTableHeader.dwGraphPointCount = graph().header().vertex_count();
	//tCrossTableHeader.m_level_guid = level_graph().header().guid();
	//FMemory::Memcpy(&tCrossTableHeader.m_game_guid, &LevelSpawn->SpawnGuid, sizeof(FGuid));
	LevelSpawn->GameCrossTableCells.AddDefaulted(level_graph().header().vertex_count());

	for (size_t i = 0; i < level_graph().header().vertex_count(); i++)
	{
		IGameLevelCrossTable::CCell& tCrossTableCell = LevelSpawn->GameCrossTableCells[i];
		tCrossTableCell.tGraphIndex = (GameGraph::_GRAPH_ID)m_results[i];
		check(graph().header().vertex_count() > tCrossTableCell.tGraphIndex);
		tCrossTableCell.fDistance = float(m_distances[tCrossTableCell.tGraphIndex][i]) * level_graph().header().cell_size();
	}

}


void CGameGraphBuilder::fill_neighbours		(class UStalkerLevelSpawn* LevelSpawn,const u32 &game_vertex_id)
{
	m_marks.assign						(level_graph().header().vertex_count(),false);
	m_current_fringe.clear				();

	u32									level_vertex_id = graph().vertex(game_vertex_id)->data().level_vertex_id();

	ILevelGraph::const_iterator			I, E;
	m_mark_stack.reserve				(8192);
	m_mark_stack.push_back				(level_vertex_id);

	for ( ; !m_mark_stack.empty(); ) {
		level_vertex_id					= m_mark_stack.back();
		m_mark_stack.resize				(m_mark_stack.size() - 1);
		const ILevelGraph::CVertex		*node = level_graph().vertex(level_vertex_id);
		level_graph().begin				(level_vertex_id,I,E);
		m_marks[level_vertex_id]		= true;
		for ( ; I != E; ++I) {
			u32							next_level_vertex_id = node->link(I);
			if (!level_graph().valid_vertex_id(next_level_vertex_id))
				continue;
			
			if (m_marks[next_level_vertex_id])
				continue;

			GameGraph::_GRAPH_ID		next_game_vertex_id = LevelSpawn->GameCrossTableCells[next_level_vertex_id].game_vertex_id();
			check						(next_game_vertex_id < graph().vertices().size());
			if (next_game_vertex_id != (GameGraph::_GRAPH_ID)game_vertex_id) {
				if	(
						std::find(
							m_current_fringe.begin(),
							m_current_fringe.end(),
							next_game_vertex_id
						)
						==
						m_current_fringe.end()
					)
					m_current_fringe.push_back	(next_game_vertex_id);
				continue;
			}

			m_mark_stack.push_back		(next_level_vertex_id);
		}
	}
}

float CGameGraphBuilder::path_distance		(const u32 &game_vertex_id0, const u32 &game_vertex_id1)
{


	graph_type::CVertex		&vertex0 = *graph().vertex(game_vertex_id0);
	graph_type::CVertex		&vertex1 = *graph().vertex(game_vertex_id1);

	using CStraightLineParams = GraphEngineSpace::CStraightLineParams;
	CStraightLineParams		parameters(vertex0.data().level_point(),vertex1.data().level_point());

	float					pure_distance = vertex0.data().level_point().distance_to_xz(vertex1.data().level_point());
	check					(pure_distance < parameters.max_range);

	u32						level_vertex_id = level_graph().check_position_in_direction(vertex0.data().level_vertex_id(),vertex0.data().level_point(),vertex1.data().level_point());
	if (level_graph().valid_vertex_id(level_vertex_id))
		return				(pure_distance);

	bool					successfull = 
		m_graph_engine->search(
			level_graph(),
			vertex0.data().level_vertex_id(),
			vertex1.data().level_vertex_id(),
			&m_path,
			parameters
		);

	if (successfull)
		return				(parameters.m_distance);

	UE_LOG(LogXRayGameGraphBuilder, Warning, TEXT("Cannot build path from [%d] to [%d]"),game_vertex_id0,game_vertex_id1);
	UE_LOG(LogXRayGameGraphBuilder, Warning, TEXT("Cannot build path from [%f][%f][%f] to [%f][%f][%f]"),VPUSH(vertex0.data().level_point()),VPUSH(vertex1.data().level_point()));

	return					(flt_max);
}

void CGameGraphBuilder::generate_edges		(const u32 &game_vertex_id)
{
	graph_type::CVertex		*vertex = graph().vertex(game_vertex_id);

	xr_vector<u32>::const_iterator	I = m_current_fringe.begin();
	xr_vector<u32>::const_iterator	E = m_current_fringe.end();
	for ( ; I != E; ++I) {
		check				(!vertex->edge(*I));
		float				distance = path_distance(game_vertex_id,*I);
		graph().add_edge	(game_vertex_id,*I,distance);
	}
}

void CGameGraphBuilder::generate_edges		(class UStalkerLevelSpawn* LevelSpawn)
{
	

	UE_LOG(LogXRayGameGraphBuilder, Log, TEXT("Generating edges"));
	
	graph_type::const_vertex_iterator	I = graph().vertices().begin();
	graph_type::const_vertex_iterator	E = graph().vertices().end();
	for ( ; I != E; ++I) {
		fill_neighbours		(LevelSpawn,(*I).second->vertex_id());
		generate_edges		((*I).second->vertex_id());
	}

	UE_LOG(LogXRayGameGraphBuilder, Log, TEXT("%d edges built"),graph().edge_count());

}

void CGameGraphBuilder::connectivity_check	()
{
	UE_LOG(LogXRayGameGraphBuilder, Log, TEXT("Checking graph connectivity"));
}

void CGameGraphBuilder::create_tripples		()
{
	m_tripples.erase(m_tripples.begin(), m_tripples.end());
	graph_type::const_vertex_iterator	I = graph().vertices().begin();
	graph_type::const_vertex_iterator	E = graph().vertices().end();
	for ( ; I != E; ++I) {
		graph_type::const_iterator	i = (*I).second->edges().begin();
		graph_type::const_iterator	e = (*I).second->edges().end();
		for ( ; i != e; ++i) {
			if (((*i).vertex_id() < (*I).first) && graph().edge((*i).vertex_id(),(*I).first))
				continue;

			const graph_type::CEdge	*edge = graph().vertex((*i).vertex_id())->edge((*I).first);

			m_tripples.push_back	(
				std::make_pair(
					_min(
						(*i).weight(),
						edge ? edge->weight() : (*i).weight()
					),
					std::make_pair(
						(*I).first,
						(*i).vertex_id()
					)
				)
			);
		}
	}

	std::sort				(m_tripples.begin(),m_tripples.end(),sort_predicate_greater<TRIPPLE>);
}

void CGameGraphBuilder::process_tripple		(const TRIPPLE &tripple)
{
	const graph_type::CVertex	&vertex0 = *graph().vertex(tripple.second.first);
	const graph_type::CVertex	&vertex1 = *graph().vertex(tripple.second.second);

	graph_type::const_iterator	I = vertex0.edges().begin();
	graph_type::const_iterator	E = vertex0.edges().end();
	for ( ; I != E; ++I) {
		if ((*I).vertex_id() == tripple.second.second)
			continue;

		const graph_type::CEdge	*edge;

		edge					= vertex1.edge((*I).vertex_id());
		if (edge) {
			check				(_min((*I).weight(),graph().edge((*I).vertex_id(),tripple.second.first) ? graph().edge((*I).vertex_id(),tripple.second.first)->weight() : (*I).weight()) <= tripple.first);
			check				(_min(edge->weight(),graph().edge(edge->vertex_id(),tripple.second.second) ? graph().edge(edge->vertex_id(),tripple.second.second)->weight() : (*I).weight()) <= tripple.first);
			if (vertex0.edge(tripple.second.second))
				graph().remove_edge	(tripple.second.first,tripple.second.second);
			if (vertex1.edge(tripple.second.first))
				graph().remove_edge	(tripple.second.second,tripple.second.first);
			return;
		}

		edge					= graph().vertex((*I).vertex_id())->edge(tripple.second.second);
		if (edge) {
			check				(_min((*I).weight(),graph().edge((*I).vertex_id(),tripple.second.first) ? graph().edge((*I).vertex_id(),tripple.second.first)->weight() : (*I).weight()) <= tripple.first);
			check				(edge->weight() <= tripple.first);
			if (vertex0.edge(tripple.second.second))
				graph().remove_edge	(tripple.second.first,tripple.second.second);
			if (vertex1.edge(tripple.second.first))
				graph().remove_edge	(tripple.second.second,tripple.second.first);
			return;
		}
	}
	return;
}

void CGameGraphBuilder::optimize_graph		()
{

	UE_LOG(LogXRayGameGraphBuilder, Log, TEXT("Optimizing graph"));

	UE_LOG(LogXRayGameGraphBuilder, Log, TEXT("edges before optimization : %d"),graph().edge_count());

	create_tripples				();

	TRIPPLES::const_iterator	I = m_tripples.begin();
	TRIPPLES::const_iterator	E = m_tripples.end();
	for ( ; I != E; ++I)
		process_tripple			(*I);

	UE_LOG(LogXRayGameGraphBuilder, Log, TEXT("edges after optimization : %d"),graph().edge_count());

}


void CGameGraphBuilder::build_game_graph(class UStalkerLevelSpawn* LevelSpawn)
{
	

	UE_LOG(LogXRayGameGraphBuilder, Log, TEXT("Building graph"));

	CTimer					timer;
	timer.Start				();

	m_graph_engine			= new CGraphEngineEditor(level_graph().header().vertex_count());

	generate_edges			(LevelSpawn);

	delete					m_graph_engine;

	connectivity_check		();
	optimize_graph			();

}

bool CGameGraphBuilder::build_graph	( UStalkerLevelSpawn* LevelSpawn)
{
	UE_LOG(LogXRayGameGraphBuilder, Log, TEXT("Building level game graph"));

	if (graph().vertices().size() == 0)
	{
		UE_LOG(LogXRayGameGraphBuilder, Error, TEXT("GameGraph points not found on level!"));
		return false;
	}
	UE_LOG(LogXRayGameGraphBuilder,Log,TEXT("%d graph points loaded"), graph().vertices().size());
	build_cross_table				(LevelSpawn);
	build_game_graph				(LevelSpawn);


	/*IGameGraph::CHeader&GameGraphHeader = LevelSpawn->GameGraphHeader;
	GameGraphHeader.m_version = XRAI_CURRENT_VERSION; 
	check(graph().vertices().size() < (u32(1) << (8*sizeof(GameGraph::_GRAPH_ID))));
	GameGraphHeader.m_vertex_count =
	check(graph().edge_count() < (u32(1) << (8*sizeof(GameGraph::_GRAPH_ID))));
	GameGraphHeader.m_edge_count =;
	GameGraphHeader.m_death_point_count	= 0;
	FMemory::Memcpy(&GameGraphHeader.m_guid,& LevelSpawn->SpawnGuid,sizeof(FGuid));

	IGameGraph::SLevel			level;
	level.m_offset.set(0, 0, 0);
	level.m_id = 0;
	level.m_name = "editor";
	level.m_section = "";
	level.m_guid = level_graph().header().guid();
	GameGraphHeader.m_levels.clear();
	GameGraphHeader.m_levels.insert(std::make_pair(level.m_id, level));*/

	LevelSpawn->GameGraphVertices.AddDefaulted(graph().vertices().size());
	LevelSpawn->GameGraphEdges.AddDefaulted(graph().edge_count());

	{
		u32								edge_offset = 0;
		size_t index = 0;
		graph_type::const_vertex_iterator	I = graph().vertices().begin();
		graph_type::const_vertex_iterator	E = graph().vertices().end();
		for (; I != E; ++I) 
		{
			IGameGraph::CVertex& vertex = (*I).second->data();
			check((*I).second->edges().size() < 256);
			vertex.tNeighbourCount = (u8)(*I).second->edges().size();
			vertex.dwEdgeOffset = edge_offset;
			LevelSpawn->GameGraphVertices[index++] = vertex;
			edge_offset += vertex.tNeighbourCount;
		}
	}
	size_t count_edges = 0;
	{
		graph_type::const_vertex_iterator	I = graph().vertices().begin();
		graph_type::const_vertex_iterator	E = graph().vertices().end();
		for (; I != E; ++I) {
			count_edges += (*I).second->edges().size();
		}
	}
	{
		size_t index = 0;
		graph_type::const_vertex_iterator	I = graph().vertices().begin();
		graph_type::const_vertex_iterator	E = graph().vertices().end();
		for (; I != E; ++I) {
			graph_type::const_iterator	i = (*I).second->edges().begin();
			graph_type::const_iterator	e = (*I).second->edges().end();
			for (; i != e; ++i) {
				IGameGraph::CEdge&edge = LevelSpawn->GameGraphEdges[index++];
				check((*i).vertex_id() < (u32(1) << (8 * sizeof(GameGraph::_GRAPH_ID))));
				edge.m_vertex_id = (GameGraph::_GRAPH_ID)(*i).vertex_id();
				edge.m_path_distance = (*i).weight();

			}
		}
	}
	UE_LOG(LogXRayGameGraphBuilder, Log, TEXT("Level graph is generated successfully"));
	return true;
}
