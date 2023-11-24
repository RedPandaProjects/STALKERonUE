#include "StalkerSpawnProperties_ALifeSpaceRestrictor.h"
#include "../../../../StalkerEditorManager.h"
#include "../../../../Managers/SEFactory/StalkerSEFactoryManager.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"
#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAssetUserData.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAnimsData.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAnimAssetUserData.h"

void UStalkerSpawnProperties_ALifeSpaceRestrictor::SetEntity(ISE_Abstract* InEntity)
{
	ALifeSpaceRestrictor = nullptr;

	if (InEntity)
	{
		ALifeSpaceRestrictor = reinterpret_cast<ISE_ALifeSpaceRestrictor*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeSpaceRestrictor));
		check(ALifeSpaceRestrictor);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeSpaceRestrictor::FillProperties()
{
	Super::FillProperties();

	CheckForSeparator =  ALifeObject->m_flags.is(ISE_ALifeObject::flCheckForSeparator);

	if (!ALifeSpaceRestrictor)
	{
		return;
	}

	RestrictionSpace::ERestrictorTypes InType = (RestrictionSpace::ERestrictorTypes)ALifeSpaceRestrictor->m_space_restrictor_type;

	switch (InType)
	{
	default:
		RetrictorType = EStalkerDefaulRetrictorTypes::NOTARestrictor;
		break;
	case RestrictionSpace::eDefaultRestrictorTypeNone:
		RetrictorType = EStalkerDefaulRetrictorTypes::NONEDefaultRestrictor;
		break;
	case RestrictionSpace::eDefaultRestrictorTypeOut:
		RetrictorType = EStalkerDefaulRetrictorTypes::OUTDefaultRestrictor;
		break;
	case RestrictionSpace::eDefaultRestrictorTypeIn:
		RetrictorType = EStalkerDefaulRetrictorTypes::INDefaultRestrictor;
		break;
	}
}

void UStalkerSpawnProperties_ALifeSpaceRestrictor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property||!ALifeObject||!ALifeSpaceRestrictor)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeSpaceRestrictor, CheckForSeparator))
	{
		ALifeObject->m_flags.set(ISE_ALifeObject::flCheckForSeparator, CheckForSeparator);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeSpaceRestrictor, RetrictorType))
	{
		switch (RetrictorType)
		{
		case EStalkerDefaulRetrictorTypes::NONEDefaultRestrictor:
			ALifeSpaceRestrictor->m_space_restrictor_type = RestrictionSpace::eDefaultRestrictorTypeNone;
			break;
		case EStalkerDefaulRetrictorTypes::OUTDefaultRestrictor:
			ALifeSpaceRestrictor->m_space_restrictor_type = RestrictionSpace::eDefaultRestrictorTypeOut;
			break;
		case EStalkerDefaulRetrictorTypes::INDefaultRestrictor:
			ALifeSpaceRestrictor->m_space_restrictor_type = RestrictionSpace::eDefaultRestrictorTypeIn;
			break;
		default:
			ALifeSpaceRestrictor->m_space_restrictor_type = RestrictionSpace::eRestrictorTypeNone;
			break;
		}
	}
}

void UStalkerSpawnProperties_ALifeLevelChanger::SetEntity(ISE_Abstract* InEntity)
{
	ALifeLevelChanger = nullptr;

	if (InEntity)
	{
		ALifeLevelChanger = reinterpret_cast<ISE_ALifeLevelChanger*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeLevelChanger));
		check(ALifeLevelChanger);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeLevelChanger::FillProperties()
{
	Super::FillProperties();
	SilentMode = 	ALifeLevelChanger->m_bSilentMode;
	PointName  =	ALifeLevelChanger->m_caLevelPointToChange.c_str();
	SetMap();
}

void UStalkerSpawnProperties_ALifeLevelChanger::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	auto CheckLevel = [this](FSoftObjectPath InLevel, FName& InName)->bool
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
	if (!PropertyChangedEvent.Property || !ALifeLevelChanger)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeLevelChanger, SilentMode))
	{
		ALifeLevelChanger->m_bSilentMode = SilentMode;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeLevelChanger, PointName))
	{
		ALifeLevelChanger->m_caLevelPointToChange = TCHAR_TO_ANSI(*PointName);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeLevelChanger, Map))
	{
		FName MapName;
		if (CheckLevel(Map, MapName))
		{
			ALifeLevelChanger->m_caLevelToChange = TCHAR_TO_ANSI(*MapName.ToString());
		}
		else
		{
			SetMap();
		}
	}
}

void UStalkerSpawnProperties_ALifeLevelChanger::SetMap()
{
	auto FindLevel = [this](FName  LevelName)->FSoftObjectPath
	{
		for (const auto& [Name, Level] : GetDefault<UStalkerGameSettings>()->GetCurrentLevels())
		{
			if (LevelName == Name)
			{
				return Level.Map;
			}
		}
		return FSoftObjectPath();
	};

	Map = FindLevel(ALifeLevelChanger->m_caLevelToChange.c_str());
}

void UStalkerSpawnProperties_ALifeCustomZone::SetEntity(ISE_Abstract* InEntity)
{
	ALifeCustomZone = nullptr;

	if (InEntity)
	{
		ALifeCustomZone = reinterpret_cast<ISE_ALifeCustomZone*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeCustomZone));
		check(ALifeCustomZone);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeCustomZone::FillProperties()
{
	Super::FillProperties();
	EnabledTime = ALifeCustomZone->m_enabled_time;
	DisabledTime = ALifeCustomZone->m_disabled_time;
	ShiftTime = ALifeCustomZone->m_start_time_shift;
}

void UStalkerSpawnProperties_ALifeCustomZone::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property || !ALifeCustomZone)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeCustomZone, EnabledTime))
	{
		ALifeCustomZone->m_enabled_time = EnabledTime;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeCustomZone, DisabledTime))
	{
		ALifeCustomZone->m_disabled_time = DisabledTime;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeCustomZone, ShiftTime))
	{
		ALifeCustomZone->m_start_time_shift = ShiftTime;
	}

}

void UStalkerSpawnProperties_ALifeAnomalousZone::SetEntity(ISE_Abstract* InEntity)
{
	ALifeAnomalousZone = nullptr;

	if (InEntity)
	{
		ALifeAnomalousZone = reinterpret_cast<ISE_ALifeAnomalousZone*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeAnomalousZone));
		check(ALifeAnomalousZone);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeAnomalousZone::FillProperties()
{
	Super::FillProperties();
	OfflineInteractiveRadius = ALifeAnomalousZone->m_offline_interactive_radius;
	ArtefactSpawnCount = ALifeAnomalousZone->m_artefact_spawn_count;
	VisibleForAI = ALifeObject->m_flags.is(ISE_ALifeObject::flVisibleForAI);
}

void UStalkerSpawnProperties_ALifeAnomalousZone::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property || !ALifeAnomalousZone || !ALifeObject)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeAnomalousZone, OfflineInteractiveRadius))
	{
		ALifeAnomalousZone->m_offline_interactive_radius = OfflineInteractiveRadius;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeAnomalousZone, ArtefactSpawnCount))
	{
		ALifeAnomalousZone->m_artefact_spawn_count = ArtefactSpawnCount;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeAnomalousZone, VisibleForAI))
	{
		ALifeObject->m_flags.set(ISE_ALifeObject::flVisibleForAI, VisibleForAI);
	}
}

void UStalkerSpawnProperties_ALifeZoneVisual::SetEntity(ISE_Abstract* InEntity)
{
	ALifeZoneVisual = nullptr;

	if (InEntity)
	{
		ALifeZoneVisual = reinterpret_cast<ISE_ALifeZoneVisual*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeZoneVisual));
		check(ALifeZoneVisual);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeZoneVisual::FillProperties()
{
	Super::FillProperties();
	SetAnim();
}

void UStalkerSpawnProperties_ALifeZoneVisual::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	auto CheckAnim = [](class UAnimSequence* Anim, FName& Name, UStalkerKinematicsAssetUserData* Kinematics)->bool
	{
		if (Kinematics)
		{
			for (const UStalkerKinematicsAnimsData* AnimsData : Kinematics->Anims)
			{
				for (const auto& [Key, InAnim] : AnimsData->Anims)
				{
					if (InAnim == Anim)
					{
						Name = Key;
						return true;
					}
				}
			}
		}
		return false;
	};
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property || !ALifeZoneVisual)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeZoneVisual, AttackAnimation))
	{
		check(Entity->visual());
		USkeletalMesh* Kinematics = GStalkerEngineManager->GetResourcesManager()->GetKinematics(Entity->visual()->get_visual());
		FName AnimName;
		if (Kinematics&&CheckAnim(AttackAnimation, AnimName, Kinematics->GetAssetUserData<UStalkerKinematicsAssetUserData>()))
		{
			ALifeZoneVisual->attack_animation = TCHAR_TO_ANSI(*AnimName.ToString().ToLower());
		}
		else
		{
			SetAnim();
		}
	}
}

void UStalkerSpawnProperties_ALifeZoneVisual::SetAnim()
{
	check(Entity->visual());
	USkeletalMesh*  Kinematics = GStalkerEngineManager->GetResourcesManager()->GetKinematics(Entity->visual()->get_visual());
	auto FindAnim = [Kinematics](FName  StartupAnimation)->class UAnimSequence*
	{
		if (Kinematics)
		{
			if(UStalkerKinematicsAssetUserData*StalkerKinematicsAssetUserData = Kinematics->GetAssetUserData<UStalkerKinematicsAssetUserData>())
			{
				for (const UStalkerKinematicsAnimsData* AnimsData : StalkerKinematicsAssetUserData->Anims)
				{
					for (const auto& [Key, InAnim] : AnimsData->Anims)
					{
						if (Key == StartupAnimation)
						{
							return InAnim;
						}
					}
				}
			}
		}
		return nullptr;
	};

	AttackAnimation = FindAnim(ALifeZoneVisual->attack_animation.c_str());
}
