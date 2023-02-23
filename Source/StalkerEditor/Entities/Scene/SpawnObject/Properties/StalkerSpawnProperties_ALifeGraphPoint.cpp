#include "StalkerSpawnProperties_ALifeGraphPoint.h"
#include "../../../../StalkerEditorManager.h"
#include "../../../../Managers/SEFactory/StalkerSEFactoryManager.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"

void UStalkerSpawnProperties_ALifeGraphPoint::SetEntity(ISE_Abstract* InEntity)
{
	ALifeGraphPoint = nullptr;

	if (InEntity)
	{
		ALifeGraphPoint = reinterpret_cast<ISE_ALifeGraphPoint*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::ALifeGraphPoint));
		check(ALifeGraphPoint);
	}

	UStalkerSpawnProperties_Base::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeGraphPoint::FillProperties()
{
	UStalkerSpawnProperties_Base::FillProperties();
	Location1.Value = TEXT("");
	const FString* Location1Key = GStalkerEditorManager->SEFactoryManager->Location1.FindKey(static_cast<int32>(ALifeGraphPoint->m_tLocations[0]));
	if (Location1Key)
	{
		Location1.Value = *Location1Key;
	}

	Location2.Value = TEXT("");
	const FString* Location2Key = GStalkerEditorManager->SEFactoryManager->Location2.FindKey(static_cast<int32>(ALifeGraphPoint->m_tLocations[1]));
	if (Location2Key)
	{
		Location2.Value = *Location2Key;
	}

	Location3.Value = TEXT("");
	const FString* Location3Key = GStalkerEditorManager->SEFactoryManager->Location3.FindKey(static_cast<int32>(ALifeGraphPoint->m_tLocations[2]));
	if (Location3Key)
	{
		Location3.Value = *Location3Key;
	}

	Location4.Value = TEXT("");
	const FString* Location4Key = GStalkerEditorManager->SEFactoryManager->Location4.FindKey(static_cast<int32>(ALifeGraphPoint->m_tLocations[3]));
	if (Location4Key)
	{
		Location4.Value = *Location4Key;
	}

	PointName = ALifeGraphPoint->m_caConnectionPointName.c_str();
	SetMap();


}

void UStalkerSpawnProperties_ALifeGraphPoint::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	auto CheckLevel= [this](FSoftObjectPath InLevel, FName& InName)->bool
	{
		FSoftObjectPath CurrentWorld = GetWorld();
		for (const auto& [Name, Level] : GetDefault<UStalkerGameSettings>()->GetCurrentLevels())
		{
			if (Level.Map == CurrentWorld)
			{
				continue;
			}
			if (Level.Map == InLevel)
			{
				InName = Name;
				return true;
			}
		}
		return false;
	};

	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property || !ALifeGraphPoint)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeGraphPoint, PointName))
	{
		ALifeGraphPoint->m_caConnectionPointName = TCHAR_TO_ANSI(*PointName);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeGraphPoint, Map))
	{
		if (Map.IsNull())
		{
			ALifeGraphPoint->m_caConnectionLevelName = "";
			ALifeGraphPoint->m_caConnectionPointName = "";
			PointName = TEXT("");
		}
		else
		{
			FName MapName;
			if (CheckLevel(Map, MapName))
			{
				ALifeGraphPoint->m_caConnectionLevelName = TCHAR_TO_ANSI(*MapName.ToString());
			}
			else
			{
				SetMap();
			}
		}
	
	}

	if (!PropertyChangedEvent.MemberProperty)
	{
		return;
	}
	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeGraphPoint, Location1))
	{
		ALifeGraphPoint->m_tLocations[0] = 255;
		int32* ID = GStalkerEditorManager->SEFactoryManager->Location1.Find(Location1.Value);
		if (ID)
		{
			ALifeGraphPoint->m_tLocations[0] = static_cast<u8>(*ID);
		}
	}
	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeGraphPoint, Location2))
	{
		ALifeGraphPoint->m_tLocations[1] = 255;
		int32* ID = GStalkerEditorManager->SEFactoryManager->Location2.Find(Location2.Value);
		if (ID)
		{
			ALifeGraphPoint->m_tLocations[1] = static_cast<u8>(*ID);
		}
	}
	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeGraphPoint, Location3))
	{
		ALifeGraphPoint->m_tLocations[2] = 255;
		int32* ID = GStalkerEditorManager->SEFactoryManager->Location3.Find(Location3.Value);
		if (ID)
		{
			ALifeGraphPoint->m_tLocations[2] = static_cast<u8>(*ID);
		}
	}
	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeGraphPoint, Location4))
	{
		ALifeGraphPoint->m_tLocations[3] = 255;
		int32* ID = GStalkerEditorManager->SEFactoryManager->Location4.Find(Location4.Value);
		if (ID)
		{
			ALifeGraphPoint->m_tLocations[3] = static_cast<u8>(*ID);
		}
	}


}

void UStalkerSpawnProperties_ALifeGraphPoint::SetMap()
{
	auto FindLevel = [this](FName  LevelName)->FSoftObjectPath
	{
		for (const auto& [Name,Level]: GetDefault<UStalkerGameSettings>()->GetCurrentLevels())
		{
			if (LevelName == Name)
			{
				return Level.Map;
			}
		}
		return FSoftObjectPath();
	};

	Map = FindLevel(ALifeGraphPoint->m_caConnectionLevelName.c_str());
}
