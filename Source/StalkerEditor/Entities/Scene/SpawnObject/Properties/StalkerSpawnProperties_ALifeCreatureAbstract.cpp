#include "StalkerSpawnProperties_ALifeCreatureAbstract.h"
#include "../../../../StalkerEditorManager.h"
#include "../../../../Managers/SEFactory/StalkerSEFactoryManager.h"


void UStalkerSpawnProperties_ALifeTraderAbstract::SetEntity(ISE_Abstract* InEntity)
{
	Entity = InEntity;
	if (Entity)
	{
		ALifeTraderAbstract = InEntity->CastALifeTraderAbstract();
		FillProperties();
	}
}

void UStalkerSpawnProperties_ALifeTraderAbstract::FillProperties()
{
	if (!ALifeTraderAbstract)
	{
		return;
	}
	Money = ALifeTraderAbstract->m_dwMoney;
	InfiniteAmmo = ALifeTraderAbstract->m_trader_flags.is(ISE_ALifeTraderAbstract::eTraderFlagInfiniteAmmo);
	CharacterProfile.Value = ALifeTraderAbstract->m_sCharacterProfile.c_str();
}

void UStalkerSpawnProperties_ALifeTraderAbstract::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property || !ALifeTraderAbstract)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeTraderAbstract, Money))
	{
		ALifeTraderAbstract->m_dwMoney = Money;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeTraderAbstract, InfiniteAmmo))
	{
		ALifeTraderAbstract->m_trader_flags.set(ISE_ALifeTraderAbstract::eTraderFlagInfiniteAmmo, InfiniteAmmo);
	}
	if (!PropertyChangedEvent.MemberProperty)
	{
		return;
	}
	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeTraderAbstract, CharacterProfile))
	{
		ALifeTraderAbstract->m_sCharacterProfile = TCHAR_TO_ANSI(*CharacterProfile.Value);
		ALifeTraderAbstract->OnChangeProfile();
	}
}

void UStalkerSpawnProperties_ALifeCreatureAbstract::SetEntity(ISE_Abstract* InEntity)
{
	ALifeCreatureAbstract = nullptr;

	if (InEntity)
	{
		ALifeCreatureAbstract = reinterpret_cast<ISE_ALifeCreatureAbstract*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeCreatureAbstract));
		check(ALifeCreatureAbstract);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeCreatureAbstract::FillProperties()
{
	Super::FillProperties();
	Health = ALifeCreatureAbstract->fHealth;
	Team = ALifeCreatureAbstract->s_team;
	Group = ALifeCreatureAbstract->s_group;
	Squad = ALifeCreatureAbstract->s_squad;
}

void UStalkerSpawnProperties_ALifeCreatureAbstract::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property ||!ALifeCreatureAbstract)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeCreatureAbstract, Health))
	{
		ALifeCreatureAbstract->fHealth = Health;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeCreatureAbstract, Team))
	{
		ALifeCreatureAbstract->s_team = Team;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeCreatureAbstract, Group))
	{
		ALifeCreatureAbstract->s_group = Group;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeCreatureAbstract, Squad))
	{
		ALifeCreatureAbstract->s_squad = Squad;
	}
}

void UStalkerSpawnProperties_ALifeMonsterAbstract::SetEntity(ISE_Abstract* InEntity)
{
	ALifeMonsterAbstract = nullptr;

	if (InEntity)
	{
		ALifeMonsterAbstract = reinterpret_cast<ISE_ALifeMonsterAbstract*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeMonsterAbstract));
		check(ALifeMonsterAbstract);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeMonsterAbstract::FillProperties()
{
	Super::FillProperties();
	NoMoveInOffline = ALifeObject->m_flags.is(ISE_ALifeObject::flOfflineNoMove);
	UseSmartTerrainTasks = ALifeObject->m_flags.is(ISE_ALifeObject::flUseSmartTerrains);

	FString InSpaceRestrictorsSTR = ALifeMonsterAbstract->m_in_space_restrictors.c_str();
	InSpaceRestrictorsSTR.ParseIntoArray(InSpaceRestrictors, TEXT(","));

	FString OutSpaceRestrictorsSTR = ALifeMonsterAbstract->m_out_space_restrictors.c_str();
	OutSpaceRestrictorsSTR.ParseIntoArray(OutSpaceRestrictors, TEXT(","));
}

void UStalkerSpawnProperties_ALifeMonsterAbstract::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property || !ALifeCreatureAbstract || !ALifeObject)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterAbstract, NoMoveInOffline))
	{
		ALifeObject->m_flags.set(ISE_ALifeObject::flOfflineNoMove, NoMoveInOffline);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterAbstract, UseSmartTerrainTasks))
	{
		ALifeObject->m_flags.set(ISE_ALifeObject::flUseSmartTerrains, UseSmartTerrainTasks);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterAbstract, InSpaceRestrictors))
	{
		FString InSpaceRestrictorsSTR;
		for (FString& Bone : InSpaceRestrictors)
		{
			if (InSpaceRestrictorsSTR.Len())
			{
				InSpaceRestrictorsSTR.AppendChar(TEXT(','));
			}
			InSpaceRestrictorsSTR.Append(Bone);
		}
		ALifeMonsterAbstract->m_in_space_restrictors = TCHAR_TO_ANSI(*InSpaceRestrictorsSTR);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterAbstract, OutSpaceRestrictors))
	{
		FString OutSpaceRestrictorsSTR;
		for (FString& Bone : OutSpaceRestrictors)
		{
			if (OutSpaceRestrictorsSTR.Len())
			{
				OutSpaceRestrictorsSTR.AppendChar(TEXT(','));
			}
			OutSpaceRestrictorsSTR.Append(Bone);
		}
		ALifeMonsterAbstract->m_out_space_restrictors = TCHAR_TO_ANSI(*OutSpaceRestrictorsSTR);
	}
}


void UStalkerSpawnProperties_ALifeHumanAbstract::SetEntity(ISE_Abstract* InEntity)
{
	ALifeHumanAbstract = nullptr;

	if (InEntity)
	{
		ALifeHumanAbstract = reinterpret_cast<ISE_ALifeHumanAbstract*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeHumanAbstract));
		check(ALifeHumanAbstract);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeHumanAbstract::FillProperties()
{
	Super::FillProperties();
	GroupBehaviour = ALifeObject->m_flags.is(ISE_ALifeObject::flGroupBehaviour);
}

void UStalkerSpawnProperties_ALifeHumanAbstract::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property || !ALifeObject)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeHumanAbstract, GroupBehaviour))
	{
		ALifeObject->m_flags.set(ISE_ALifeObject::flGroupBehaviour, GroupBehaviour);
	}
}

void UStalkerSpawnProperties_ALifeMonsterZombie::SetEntity(ISE_Abstract* InEntity)
{
	ALifeMonsterZombie = nullptr;

	if (InEntity)
	{
		ALifeMonsterZombie = reinterpret_cast<ISE_ALifeMonsterZombie*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeMonsterZombie));
		check(ALifeMonsterZombie);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeMonsterZombie::FillProperties()
{
	Super::FillProperties();

	EyeFov = ALifeMonsterZombie->fEyeFov;
	EyeRange = ALifeMonsterZombie->fEyeRange;
	AttackAngle = ALifeMonsterZombie->fAttackAngle;
	AttackDistance = ALifeMonsterZombie->fAttackDistance;
	AttackSpeed = ALifeMonsterZombie->fAttackSpeed;
	HitPower = ALifeMonsterZombie->fHitPower;
	MaxHomeRadius = ALifeMonsterZombie->fMaxHomeRadius;
	MaxPursuitRadius = ALifeMonsterZombie->fMaxPursuitRadius;
	MaxSpeed = ALifeMonsterZombie->fMaxSpeed;
	MinSpeed = ALifeMonsterZombie->fMinSpeed;
	HitInterval = ALifeMonsterZombie->u16HitInterval;

}

void UStalkerSpawnProperties_ALifeMonsterZombie::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property || !ALifeMonsterZombie)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterZombie, EyeFov))
	{
		ALifeMonsterZombie->fEyeFov = EyeFov;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterZombie, EyeRange))
	{
		ALifeMonsterZombie->fEyeRange = EyeRange;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterZombie, MinSpeed))
	{
		ALifeMonsterZombie->fMinSpeed = MinSpeed;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterZombie, MaxSpeed))
	{
		ALifeMonsterZombie->fMaxSpeed = MaxSpeed;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterZombie, AttackSpeed))
	{
		ALifeMonsterZombie->fAttackSpeed = AttackSpeed;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterZombie, MaxPursuitRadius))
	{
		ALifeMonsterZombie->fMaxPursuitRadius = MaxPursuitRadius;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterZombie, MaxHomeRadius))
	{
		ALifeMonsterZombie->fMaxHomeRadius = MaxHomeRadius;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterZombie, HitPower))
	{
		ALifeMonsterZombie->fHitPower = HitPower;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterZombie, HitInterval))
	{
		ALifeMonsterZombie->u16HitInterval = HitInterval;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterZombie, AttackDistance))
	{
		ALifeMonsterZombie->fAttackDistance = AttackDistance;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterZombie, AttackAngle))
	{
		ALifeMonsterZombie->fAttackAngle = AttackAngle;
	}
}

void UStalkerSpawnProperties_ALifeMonsterRat::SetEntity(ISE_Abstract* InEntity)
{
	ALifeMonsterRat = nullptr;

	if (InEntity)
	{
		ALifeMonsterRat = reinterpret_cast<ISE_ALifeMonsterRat*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeMonsterRat));
		check(ALifeMonsterRat);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeMonsterRat::FillProperties()
{
	Super::FillProperties();
	EyeFov = ALifeMonsterRat->fEyeFov;
	EyeRange = ALifeMonsterRat->fEyeRange;
	AttackAngle = ALifeMonsterRat->fAttackAngle;
	AttackDistance = ALifeMonsterRat->fAttackDistance;
	AttackSpeed = ALifeMonsterRat->fAttackSpeed;
	HitPower = ALifeMonsterRat->fHitPower;
	MaxHomeRadius = ALifeMonsterRat->fMaxHomeRadius;
	MaxPursuitRadius = ALifeMonsterRat->fMaxPursuitRadius;
	MaxSpeed = ALifeMonsterRat->fMaxSpeed;
	MinSpeed = ALifeMonsterRat->fMinSpeed;
	HitInterval = ALifeMonsterRat->u16HitInterval;

	AttackSuccessProbability = ALifeMonsterRat->fAttackSuccessProbability;
	MoraleDeathQuant = ALifeMonsterRat->fMoraleDeathQuant;
	MoraleFearQuant = ALifeMonsterRat->fMoraleFearQuant;
	MoraleMaxValue = ALifeMonsterRat->fMoraleMaxValue;
	MoraleMinValue = ALifeMonsterRat->fMoraleMinValue;
	MoraleNormalValue = ALifeMonsterRat->fMoraleNormalValue;
	MoraleRestoreQuant = ALifeMonsterRat->fMoraleRestoreQuant;
	MoraleSuccessAttackQuant = ALifeMonsterRat->fMoraleSuccessAttackQuant;
	MoraleRestoreTimeInterval = ALifeMonsterRat->u16MoraleRestoreTimeInterval;

}

void UStalkerSpawnProperties_ALifeMonsterRat::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property || !ALifeMonsterRat)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, EyeFov))
	{
		ALifeMonsterRat->fEyeFov = EyeFov;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, EyeRange))
	{
		ALifeMonsterRat->fEyeRange = EyeRange;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, MinSpeed))
	{
		ALifeMonsterRat->fMinSpeed = MinSpeed;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, MaxSpeed))
	{
		ALifeMonsterRat->fMaxSpeed = MaxSpeed;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, AttackSpeed))
	{
		ALifeMonsterRat->fAttackSpeed = AttackSpeed;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, MaxPursuitRadius))
	{
		ALifeMonsterRat->fMaxPursuitRadius = MaxPursuitRadius;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, MaxHomeRadius))
	{
		ALifeMonsterRat->fMaxHomeRadius = MaxHomeRadius;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, HitPower))
	{
		ALifeMonsterRat->fHitPower = HitPower;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, HitInterval))
	{
		ALifeMonsterRat->u16HitInterval = HitInterval;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, AttackDistance))
	{
		ALifeMonsterRat->fAttackDistance = AttackDistance;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, AttackAngle))
	{
		ALifeMonsterRat->fAttackAngle = AttackAngle;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, AttackSuccessProbability))
	{
		ALifeMonsterRat->fAttackSuccessProbability = AttackSuccessProbability;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, MoraleDeathQuant))
	{
		ALifeMonsterRat->fMoraleDeathQuant = MoraleDeathQuant;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, MoraleFearQuant))
	{
		ALifeMonsterRat->fMoraleFearQuant = MoraleFearQuant;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, MoraleMaxValue))
	{
		ALifeMonsterRat->fMoraleMaxValue = MoraleMaxValue;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, MoraleMinValue))
	{
		ALifeMonsterRat->fMoraleMinValue = MoraleMinValue;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, MoraleNormalValue))
	{
		ALifeMonsterRat->fMoraleNormalValue = MoraleNormalValue;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, MoraleRestoreQuant))
	{
		ALifeMonsterRat->fMoraleRestoreQuant = MoraleRestoreQuant;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, MoraleSuccessAttackQuant))
	{
		ALifeMonsterRat->fMoraleSuccessAttackQuant = MoraleSuccessAttackQuant;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeMonsterRat, MoraleRestoreTimeInterval))
	{
		ALifeMonsterRat->u16MoraleRestoreTimeInterval = MoraleRestoreTimeInterval;
	}
}
