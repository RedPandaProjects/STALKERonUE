#pragma once
#include "StalkerSpawnProperties_ALifeObject.h"
#include "StalkerSpawnProperties_ALifeItem.generated.h"


UCLASS()
class  UStalkerSpawnProperties_ALifeItemWeapon : public UStalkerSpawnProperties_ALifeObject
{
	GENERATED_BODY()
public:
	void				SetEntity				(ISE_Abstract* InEntity) override;
	void				FillProperties			() override;
	void				PostEditChangeProperty	(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere,Category = "Weapon")
	uint8				AmmoType;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	uint16				AmmoInMagazine;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool				AddonsScope;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool				AddonsSilencer;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool				AddonsPodstvolnik;

protected:
	ISE_ALifeItemWeapon* ALifeItemWeapon;
};


UCLASS()
class  UStalkerSpawnProperties_ALifeItemAmmo : public UStalkerSpawnProperties_ALifeObject
{
	GENERATED_BODY()
public:
	void				SetEntity(ISE_Abstract* InEntity) override;
	void				FillProperties() override;
	void				PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "Ammo")
	uint16				Quantity;

protected:
	ISE_ALifeItemAmmo* ALifeItemAmmo;
};

UCLASS()
class  UStalkerSpawnProperties_ALifeItemArtefact : public UStalkerSpawnProperties_ALifeObject
{
	GENERATED_BODY()
public:
	void				SetEntity(ISE_Abstract* InEntity) override;
	void				FillProperties() override;
	void				PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "Anomaly")
	float						AnomalyValue;

protected:
	ISE_ALifeItemArtefact* ALifeItemArtefact;
};

UCLASS()
class  UStalkerSpawnProperties_ALifeItemDocument : public UStalkerSpawnProperties_ALifeObject
{
	GENERATED_BODY()
public:
	void				SetEntity(ISE_Abstract* InEntity) override;
	void				FillProperties() override;
	void				PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "Document")
	FString				InfoPortion;

protected:
	ISE_ALifeItemDocument*	ALifeItemDocument;
};
