#include "StalkerSpawnProperties_Base.h"

void UStalkerSpawnProperties_Base::SetEntity(ISE_Abstract* InEntiy)
{
	Entity = InEntiy;
	if (Entity)
	{
		FillProperties();
	}
}

void UStalkerSpawnProperties_Base::FillProperties()
{
	check(Entity);
}
