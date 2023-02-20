////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_builder_inline.h
//	Created 	: 14.12.2005
//  Modified 	: 14.12.2005
//	Author		: Dmitriy Iassenev
//	Description : Game graph builder inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	ILevelGraph& CGameGraphBuilder::level_graph() const
{
	return	(*m_level_graph);
}

IC	CGameGraphBuilder::graph_type &CGameGraphBuilder::graph	() const
{
	return	(*m_graph);
}

