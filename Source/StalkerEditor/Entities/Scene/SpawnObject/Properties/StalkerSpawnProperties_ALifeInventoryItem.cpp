#include "StalkerSpawnProperties_ALifeInventoryItem.h"
void UStalkerSpawnProperties_ALifeInventoryItem::SetEntity(ISE_Abstract* InEntity)
{
	Entity = InEntity;
	if (Entity)
	{
		ALifeInventoryItem = InEntity->CastALifeInventoryItem();
		FillProperties();
	}
} 

void UStalkerSpawnProperties_ALifeInventoryItem::FillProperties()
{
	if (!ALifeInventoryItem)
	{
		return;
	}
	ItemCondition =  ALifeInventoryItem->m_fCondition;
	ISE_ALifeObject* ALifeObject =  Entity->CastALifeObject();
	check(ALifeObject);
	UsefulForAI = ALifeObject->m_flags.is(ISE_ALifeObject::flUsefulForAI);
	VisibleForAI = ALifeObject->m_flags.is(ISE_ALifeObject::flVisibleForAI);
}

void UStalkerSpawnProperties_ALifeInventoryItem::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property|| !ALifeInventoryItem)
	{
		return;
	}
	ISE_ALifeObject* ALifeObject = Entity->CastALifeObject();
	check(ALifeObject);
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeInventoryItem, ItemCondition))
	{
		ALifeInventoryItem->m_fCondition = ItemCondition;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeInventoryItem, VisibleForAI))
	{
		ALifeObject->m_flags.set(ISE_ALifeObject::flVisibleForAI, VisibleForAI);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeInventoryItem, UsefulForAI))
	{
		ALifeObject->m_flags.set(ISE_ALifeObject::flUsefulForAI, UsefulForAI);
	}
}
