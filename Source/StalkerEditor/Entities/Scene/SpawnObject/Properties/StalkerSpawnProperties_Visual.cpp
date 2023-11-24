#include "StalkerSpawnProperties_Visual.h"
#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAssetUserData.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAnimsData.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAnimAssetUserData.h"

void UStalkerSpawnProperties_Visual::SetEntity(ISE_Abstract* InEntity)
{
	Entity = InEntity;
	if (Entity)
	{
		EntityVisual = InEntity->visual();
		FillProperties();
	}
} 

void UStalkerSpawnProperties_Visual::FillProperties()
{
	if (!EntityVisual)
	{
		return;
	}

	Kinematics = GStalkerEngineManager->GetResourcesManager()->GetKinematics(EntityVisual->get_visual());
	Obstacle = EntityVisual->flags.is(ISE_Visual::flObstacle);
	SetAnim();
}

void UStalkerSpawnProperties_Visual::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{

	auto CheckAnim = [this](class UAnimSequence* Anim,FName&Name)->bool
	{
		if (Kinematics)
		{
			if(UStalkerKinematicsAssetUserData *StalkerKinematicsAnimsData =  Kinematics->GetAssetUserData<UStalkerKinematicsAssetUserData>())
			{
				for (const UStalkerKinematicsAnimsData* AnimsData : StalkerKinematicsAnimsData->Anims)
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
		}
		return false;
	};

	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property)
	{
		return;
	}
	if (!EntityVisual)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_Visual, Kinematics))
	{
		EntityVisual->set_visual(TCHAR_TO_ANSI(* Kinematics->GetPathName()));
		EntityVisual->startup_animation = "$editor";
		SetAnim();
		Entity->set_editor_flag(ISE_Abstract::flVisualChange);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_Visual, Anim))
	{
		FName AnimName;
		if (Anim == nullptr)
		{
			EntityVisual->startup_animation = "";
			Entity->set_editor_flag(ISE_Abstract::flVisualAnimationChange);
		}
		else if (CheckAnim(Anim, AnimName))
		{
			EntityVisual->startup_animation = TCHAR_TO_ANSI(*AnimName.ToString().ToLower());
			Entity->set_editor_flag(ISE_Abstract::flVisualAnimationChange);
		}
		else
		{
			SetAnim();
		}

	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_Visual, Obstacle))
	{
		EntityVisual->flags.set(ISE_Visual::flObstacle, Obstacle);
	}
}

void UStalkerSpawnProperties_Visual::SetAnim()
{
	auto FindAnim = [this](FName  StartupAnimation)->class UAnimSequence*
	{
		if (Kinematics)
		{
			if(UStalkerKinematicsAssetUserData *StalkerKinematicsAnimsData =  Kinematics->GetAssetUserData<UStalkerKinematicsAssetUserData>())
			{
				for (const UStalkerKinematicsAnimsData* AnimsData : StalkerKinematicsAnimsData->Anims)
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

	Anim = FindAnim(EntityVisual->startup_animation.c_str());
}
