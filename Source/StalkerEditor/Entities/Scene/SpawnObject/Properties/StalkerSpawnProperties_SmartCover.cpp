#include "StalkerSpawnProperties_SmartCover.h"
#include "../../../../StalkerEditorManager.h"
#include "../../../../Managers/SEFactory/StalkerSEFactoryManager.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"
#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAssetUserData.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAnimsData.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAnimAssetUserData.h"

void UStalkerSpawnProperties_SmartCover::SetEntity(ISE_Abstract* InEntity)
{
	SmartCover = nullptr;

	if (InEntity)
	{
		SmartCover = reinterpret_cast<ISE_SmartCover*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_SmartCover));
		check(SmartCover);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_SmartCover::FillProperties()
{
	Super::FillProperties();
	SmartCover->OnFillProperties();
	Description.Value = SmartCover->m_description.c_str();
	HoldPositionTime = SmartCover->m_hold_position_time;
	ExitMinEnemyDistance = SmartCover->m_exit_min_enemy_distance;
	EnterMinEnemyDistance = SmartCover->m_enter_min_enemy_distance;
	IsCombatCover = SmartCover->m_is_combat_cover;
	CanFire = SmartCover->m_can_fire;

	Loopholes.Reset();
	for (auto& [Key, Value] : SmartCover->EditorLoopholes)
	{
		Loopholes.AddDefaulted();
		Loopholes.Last().XRayName = Key;
		Loopholes.Last().Name = Key.c_str();
		Loopholes.Last().Use  = Value;
	}


}

void UStalkerSpawnProperties_SmartCover::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property || !SmartCover)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_SmartCover, HoldPositionTime))
	{
		SmartCover->m_hold_position_time = HoldPositionTime;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_SmartCover, ExitMinEnemyDistance))
	{
		SmartCover->m_exit_min_enemy_distance = ExitMinEnemyDistance;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_SmartCover, EnterMinEnemyDistance))
	{
		SmartCover->m_enter_min_enemy_distance = EnterMinEnemyDistance;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_SmartCover, IsCombatCover))
	{
		SmartCover->m_is_combat_cover = IsCombatCover;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_SmartCover, CanFire))
	{
		SmartCover->m_can_fire = CanFire;
	}
	if (!PropertyChangedEvent.MemberProperty)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_SmartCover, Loopholes))
	{
		for (FStalkerSpawnData_Loophole& Item : Loopholes)
		{
			if (SmartCover->EditorLoopholes.contains(Item.XRayName))
			{
				SmartCover->EditorLoopholes[Item.XRayName] = Item.Use;
			}
		}
		SmartCover->OnChangeLoopholes();
	}
	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_SmartCover, Description))
	{
		SmartCover->m_description = TCHAR_TO_ANSI(*Description.Value);
		SmartCover->OnChangeDescription();
	}
}
