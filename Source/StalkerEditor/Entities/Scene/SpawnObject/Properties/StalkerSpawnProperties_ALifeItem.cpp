#include "StalkerSpawnProperties_ALifeItem.h"
#include "../../../../StalkerEditorManager.h"
#include "../../../../Managers/SEFactory/StalkerSEFactoryManager.h"

void UStalkerSpawnProperties_ALifeItemWeapon::SetEntity(ISE_Abstract* InEntity)
{
	ALifeItemWeapon = nullptr;

	if (InEntity)
	{
		ALifeItemWeapon = reinterpret_cast<ISE_ALifeItemWeapon*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeItemWeapon));
		check(ALifeItemWeapon);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeItemWeapon::FillProperties()
{
	Super::FillProperties();

	AmmoType = ALifeItemWeapon->ammo_type;
	AmmoInMagazine = ALifeItemWeapon->a_elapsed;
	AddonsScope = ALifeItemWeapon->m_addon_flags.is(ISE_ALifeItemWeapon::eWeaponAddonScope);
	AddonsSilencer = ALifeItemWeapon->m_addon_flags.is(ISE_ALifeItemWeapon::eWeaponAddonSilencer);
	AddonsPodstvolnik = ALifeItemWeapon->m_addon_flags.is(ISE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher);


}

void UStalkerSpawnProperties_ALifeItemWeapon::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property||!ALifeItemWeapon)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeItemWeapon, AmmoType))
	{
		ALifeItemWeapon->ammo_type = AmmoType;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeItemWeapon, AmmoInMagazine))
	{
		ALifeItemWeapon->a_elapsed = AmmoInMagazine;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeItemWeapon, AddonsScope))
	{
		if (ALifeItemWeapon->m_scope_status == ALife::eAddonAttachable)
		{
			ALifeItemWeapon->m_addon_flags.set(ISE_ALifeItemWeapon::eWeaponAddonScope, AddonsScope);
		}
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeItemWeapon, AddonsSilencer))
	{
		if (ALifeItemWeapon->m_silencer_status == ALife::eAddonAttachable)
		{
			ALifeItemWeapon->m_addon_flags.set(ISE_ALifeItemWeapon::eWeaponAddonSilencer, AddonsSilencer);
		}
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeItemWeapon, AddonsPodstvolnik))
	{
		if (ALifeItemWeapon->m_grenade_launcher_status == ALife::eAddonAttachable)
		{
			ALifeItemWeapon->m_addon_flags.set(ISE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher, AddonsPodstvolnik);
		}
	}
}



void UStalkerSpawnProperties_ALifeItemAmmo::SetEntity(ISE_Abstract* InEntity)
{

	ALifeItemAmmo = nullptr;

	if (InEntity)
	{
		ALifeItemAmmo = reinterpret_cast<ISE_ALifeItemAmmo*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeItemAmmo));	
		check(ALifeItemAmmo);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeItemAmmo::FillProperties()
{
	Super::FillProperties();
	Quantity = ALifeItemAmmo->a_elapsed;


}

void UStalkerSpawnProperties_ALifeItemAmmo::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property||!ALifeItemAmmo)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeItemAmmo, Quantity))
	{
		ALifeItemAmmo->a_elapsed = Quantity;
	}
}




void UStalkerSpawnProperties_ALifeItemArtefact::SetEntity(ISE_Abstract* InEntity)
{

	ALifeItemArtefact = nullptr;

	if (InEntity)
	{
		ALifeItemArtefact = reinterpret_cast<ISE_ALifeItemArtefact*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeItemArtefact));
		check(ALifeItemArtefact);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeItemArtefact::FillProperties()
{
	Super::FillProperties();
	AnomalyValue = ALifeItemArtefact->m_fAnomalyValue;


}

void UStalkerSpawnProperties_ALifeItemArtefact::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property||!ALifeItemArtefact)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeItemArtefact, AnomalyValue))
	{
		ALifeItemArtefact->m_fAnomalyValue = AnomalyValue;
	}
}



void UStalkerSpawnProperties_ALifeItemDocument::SetEntity(ISE_Abstract* InEntity)
{

	ALifeItemDocument = nullptr;

	if (InEntity)
	{
		ALifeItemDocument = reinterpret_cast<ISE_ALifeItemDocument*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeItemDocument));
		check(ALifeItemDocument);
	}

	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeItemDocument::FillProperties()
{
	Super::FillProperties();

	InfoPortion = ALifeItemDocument->m_wDoc.c_str();


}

void UStalkerSpawnProperties_ALifeItemDocument::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property||!ALifeItemDocument)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeItemDocument, InfoPortion))
	{
		ALifeItemDocument->m_wDoc = TCHAR_TO_ANSI(*InfoPortion);
	}
}

