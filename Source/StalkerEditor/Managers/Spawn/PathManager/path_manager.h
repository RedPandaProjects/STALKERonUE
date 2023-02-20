////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Generic/path_manager_generic.h"

template <
	typename _Graph,
	typename _DataStorage,
	typename _Parameters,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager : 
		public CPathManagerGeneric <
			_Graph,
			_DataStorage,
			_Parameters,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
};

//		path manager parameters
#include "Params/path_manager_params.h"
#include "Params/path_manager_params_flooder.h"
#include "Params/path_manager_params_straight_line.h"

//		path manager specializations
#include "Game/path_manager_game.h"


#include "Game/Level/path_manager_game_level.h"

#include "Level/path_manager_level.h"
#include "Level/Flooder/path_manager_level_flooder.h"

#include "Level/StraightLine/path_manager_level_straight_line.h"