////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_engine.h
//	Created 	: 21.03.2002
//  Modified 	: 26.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Graph engine
////////////////////////////////////////////////////////////////////////////

#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrEngine/a_star.h"
#include "XrEngine/edge_path.h"
#include "XrEngine/vertex_manager_fixed.h"
#include "XrEngine/vertex_manager_hash_fixed.h"
#include "XrEngine/vertex_allocator_fixed.h"
#include "XrEngine/data_storage_bucket_list.h"
#include "XrEngine/data_storage_binary_heap.h"
THIRD_PARTY_INCLUDES_END
#include "../../PathManager/path_manager.h"
#include "graph_engine_editor_space.h"

using namespace GraphEngineSpace;

class CGraphEngineEditor
{
public:
	typedef CDataStorageBucketList<u32,u32,8*1024,false>	CPriorityQueue;
	
	typedef CVertexManagerFixed<u32,u32,8>					CVertexManager;

	typedef CVertexAllocatorFixed<2*1024*1024>				CVertexAllocator;

	typedef CAStar<
		_dist_type,
		CPriorityQueue,
		CVertexManager,
		CVertexAllocator
	>														CAlgorithm;
	

	CAlgorithm				*m_algorithm;

public:

	IC				CGraphEngineEditor			(u32 max_vertex_count);
	virtual			~CGraphEngineEditor			();

	template <
		typename _Graph,
		typename _Parameters
	>
	IC		bool	search					(
				const _Graph			&graph, 
				const _index_type		&start_node, 
				const _index_type		&dest_node, 
				xr_vector<_index_type>	*node_path,
				const _Parameters		&parameters
			);

	template <
		typename _Graph,
		typename _Parameters
	>
	IC		bool	search					(
				const _Graph			&graph, 
				const _index_type		&start_node, 
				const _index_type		&dest_node, 
				xr_vector<_index_type>	*node_path,
				_Parameters				&parameters
			);

	template <
		typename _Graph,
		typename _Parameters,
		typename _PathManager
	>
	IC		bool	search					(
				const _Graph			&graph, 
				const _index_type		&start_node, 
				const _index_type		&dest_node, 
				xr_vector<_index_type>	*node_path,
				const _Parameters		&parameters,
				_PathManager			&path_manager
			);
};

#include "graph_engine_editor_inline.h"