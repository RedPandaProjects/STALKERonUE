#include "StalkerSpawnProperties_ALifeObject.h"
#include "../../../../StalkerEditorManager.h"
#include "../../../../Managers/SEFactory/StalkerSEFactoryManager.h"

void UStalkerSpawnProperties_ALifeObject::SetEntity(ISE_Abstract* InEntity)
{
	ALifeObject = nullptr;
	
	if (InEntity)
	{
		ALifeObject = reinterpret_cast<ISE_ALifeObject*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::ALifeObject));
		check(ALifeObject);
	}

	UStalkerSpawnProperties_Base::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeObject::FillProperties()
{
	UStalkerSpawnProperties_Base::FillProperties();
	CustomData			= Entity->m_ini_string.c_str();
	CanSwitchOnline		= ALifeObject->m_flags.is(ISE_ALifeObject::flSwitchOnline);
	CanSwitchOffline	= ALifeObject->m_flags.is(ISE_ALifeObject::flSwitchOffline);
	Interactive			= ALifeObject->m_flags.is(ISE_ALifeObject::flInteractive);
	UsedAILocations		= ALifeObject->m_flags.is(ISE_ALifeObject::flUsedAI_Locations);

	Story.Value = TEXT("");
	const FString* StorysKey =   GStalkerEditorManager->SEFactoryManager->Storys.FindKey(static_cast<int32>(ALifeObject->m_story_id));
	if (StorysKey)
	{
		Story.Value = *StorysKey;
	}

	SpawnStory.Value = TEXT("");
	const FString* SpawnStorysKey = GStalkerEditorManager->SEFactoryManager->SpawnStorys.FindKey(static_cast<int32>(ALifeObject->m_spawn_story_id));
	if (SpawnStorysKey)
	{
		SpawnStory.Value = *SpawnStorysKey;
	}
}

void UStalkerSpawnProperties_ALifeObject::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property||!Entity||!ALifeObject)
	{
		return;
	}
	if ( PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObject, CustomData))
	{
		Entity->m_ini_string = TCHAR_TO_ANSI(*CustomData);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObject, CanSwitchOnline))
	{
		ALifeObject->m_flags.set(ISE_ALifeObject::flSwitchOnline, CanSwitchOnline);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObject, CanSwitchOffline))
	{
		ALifeObject->m_flags.set(ISE_ALifeObject::flSwitchOffline, CanSwitchOffline);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObject, Interactive))
	{
		ALifeObject->m_flags.set(ISE_ALifeObject::flInteractive, Interactive);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObject, UsedAILocations))
	{
		ALifeObject->m_flags.set(ISE_ALifeObject::flUsedAI_Locations, UsedAILocations);
	}
	if (!PropertyChangedEvent.MemberProperty)
	{
		return;
	}
	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObject, Story))
	{
		ALifeObject->m_story_id = -1;
		int32*ID =   GStalkerEditorManager->SEFactoryManager->Storys.Find(Story.Value);
		if (ID)
		{
			ALifeObject->m_story_id = static_cast<u32>(*ID);
		}
	}
	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObject, SpawnStory))
	{
		ALifeObject->m_spawn_story_id = -1;
		int32* ID = GStalkerEditorManager->SEFactoryManager->SpawnStorys.Find(SpawnStory.Value);
		if (ID)
		{
			ALifeObject->m_spawn_story_id = static_cast<u32>(*ID);
		}
	}
}
