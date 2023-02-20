////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_storage.h
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path storage
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Resources/Spawn/StalkerLevelSpawn.h"
class CPatrolPath;
class ILevelGraph;
class IGameLevelCrossTable;
class IGameGraph;
THIRD_PARTY_INCLUDES_START
#include "xrEngine\object_interfaces.h"
#include "xrEngine\associative_vector.h"
THIRD_PARTY_INCLUDES_END
class CPatrolPathStorage : public IPureSerializeObject<IReader,IWriter> {
private:
	typedef IPureSerializeObject<IReader,IWriter>		inherited;

public:
	typedef associative_vector<shared_str,CPatrolPath*>	PATROL_REGISTRY;
	typedef PATROL_REGISTRY::iterator					iterator;
	typedef PATROL_REGISTRY::const_iterator				const_iterator;

protected:
	PATROL_REGISTRY					m_registry;

public:
	IC								CPatrolPathStorage	();
	virtual							~CPatrolPathStorage	();
	void							save				(IWriter &stream) override;
	void							load				(IReader &stream) override;

public:

			void					load_editor			(const ILevelGraph* level_graph, const IGameLevelCrossTable* cross, const IGameGraph* game_graph,const TArray<FStalkerLevelSpawnWay>&Ways);
	IC		const CPatrolPath		*path				(shared_str patrol_name, bool no_assert = false) const;
	IC		const PATROL_REGISTRY	&patrol_paths		() const;
};

#include "patrol_path_storage_inline.h"