#include "StalkerSpawnProperties_ALifeDynamicObjectVisual.h"

void UStalkerSpawnProperties_ALifeObjectPhysic::SetEntity(ISE_Abstract* InEntity)
{
	ALifeObjectPhysic = nullptr;
	if (InEntity)
	{
		ALifeObjectPhysic = reinterpret_cast<ISE_ALifeObjectPhysic*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeObjectPhysic));
		check(ALifeObjectPhysic);
	}
	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeObjectPhysic::FillProperties()
{
	Super::FillProperties();;
	switch ((EPOType)ALifeObjectPhysic->type)
	{
	case epotFixedChain:
		ObjectPhysicType = EStalkerObjectPhysicTypes::FixedChain;
		break;
	case epotFreeChain:
		ObjectPhysicType = EStalkerObjectPhysicTypes::FreeChain;
		break;
	case epotSkeleton:
		ObjectPhysicType = EStalkerObjectPhysicTypes::Skeleton;
		break;
	default:
		ObjectPhysicType = EStalkerObjectPhysicTypes::Box;
		break;

	}
	ObjectPhysicMass = ALifeObjectPhysic->mass;
	ObjectPhysicActive = ALifeObjectPhysic->GetPHSkeletonFlags8().is(ISE_ALifeObjectPhysic::flActive);
	FString FixedBonesSTR = ALifeObjectPhysic->fixed_bones.c_str();
	FixedBonesSTR.ParseIntoArray(FixedBones, TEXT(","));
}

void UStalkerSpawnProperties_ALifeObjectPhysic::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property || !ALifeObjectPhysic)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectPhysic, ObjectPhysicMass))
	{
		ALifeObjectPhysic->mass = ObjectPhysicMass;
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectPhysic, ObjectPhysicType))
	{
		switch (ObjectPhysicType)
		{
		case EStalkerObjectPhysicTypes::FixedChain:
			ALifeObjectPhysic->type = EPOType::epotFixedChain;
			break;
		case EStalkerObjectPhysicTypes::FreeChain:
			ALifeObjectPhysic->type = EPOType::epotFreeChain;
			break;
		case EStalkerObjectPhysicTypes::Skeleton:
			ALifeObjectPhysic->type = EPOType::epotSkeleton;
			break;
		default:
			ALifeObjectPhysic->type = EPOType::epotBox;
			break;
		}
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectPhysic, ObjectPhysicActive))
	{
		ALifeObjectPhysic->GetPHSkeletonFlags8().set(ISE_ALifeObjectPhysic::flActive, ObjectPhysicActive);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectPhysic, FixedBones))
	{
		FString FixedBonesSTR;
		for (FString& Bone : FixedBones)
		{
			if (FixedBonesSTR.Len())
			{
				FixedBonesSTR.AppendChar(TEXT(','));
			}
			FixedBonesSTR.Append(Bone);
		}
		ALifeObjectPhysic->fixed_bones = TCHAR_TO_ANSI(*FixedBonesSTR);
	}
}

void UStalkerSpawnProperties_ALifeHelicopter::SetEntity(ISE_Abstract* InEntity)
{
	ALifeHelicopter = nullptr;
	if (InEntity)
	{
		ALifeHelicopter = reinterpret_cast<ISE_ALifeHelicopter*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeHelicopter));
		check(ALifeHelicopter);
	}
	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeHelicopter::FillProperties()
{
	Super::FillProperties();;
	EngineSound = ALifeHelicopter->engine_sound.c_str();
}

void UStalkerSpawnProperties_ALifeHelicopter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property || !ALifeHelicopter)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeHelicopter, EngineSound))
	{
		ALifeHelicopter->engine_sound = TCHAR_TO_ANSI(*EngineSound);
	}
}

void UStalkerSpawnProperties_ALifeCar::SetEntity(ISE_Abstract* InEntity)
{
	ALifeCar = nullptr;
	if (InEntity)
	{
		ALifeCar = reinterpret_cast<ISE_ALifeCar*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeCar));
		check(ALifeCar);
	}
	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeCar::FillProperties()
{
	Super::FillProperties();;
	Health = ALifeCar->health;
}

void UStalkerSpawnProperties_ALifeCar::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property || !ALifeCar)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeCar, Health))
	{
		ALifeCar->health = Health;
	}
}

void UStalkerSpawnProperties_ALifeObjectBreakable::SetEntity(ISE_Abstract* InEntity)
{
	ALifeObjectBreakable = nullptr;
	if (InEntity)
	{
		ALifeObjectBreakable = reinterpret_cast<ISE_ALifeObjectBreakable*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeObjectBreakable));
		check(ALifeObjectBreakable);
	}
	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeObjectBreakable::FillProperties()
{
	Super::FillProperties();;
	Health = ALifeObjectBreakable->m_health;
}

void UStalkerSpawnProperties_ALifeObjectBreakable::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property||!ALifeObjectBreakable)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectBreakable, Health))
	{
		ALifeObjectBreakable->m_health = Health;
	}
}

