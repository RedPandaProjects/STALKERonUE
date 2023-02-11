#pragma once
#include "StalkerSpawnProperties_Base.h"
#include "StalkerSpawnPropertiesTypes.h"
#include "StalkerSpawnProperties_ALifeObject.h"
#include "StalkerSpawnProperties_ALifeCreatureAbstract.generated.h"

UCLASS(DefaultToInstanced)
class  UStalkerSpawnProperties_ALifeTraderAbstract : public UObject
{
	GENERATED_BODY()
public:
	void							SetEntity(ISE_Abstract* InEntity);
	void							FillProperties();
	void							PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "NPC")
	uint32							Money;
	UPROPERTY(EditAnywhere, Category = "NPC")
	bool							InfiniteAmmo;
	UPROPERTY(EditAnywhere, Category = "NPC")
	FStalkerSpawnData_CharacterProfile	CharacterProfile;

protected:
	ISE_Abstract*					Entity = nullptr;
	ISE_ALifeTraderAbstract*		ALifeTraderAbstract = nullptr;
};


UCLASS()
class  UStalkerSpawnProperties_ALifeCreatureAbstract : public UStalkerSpawnProperties_ALifeObject
{
	GENERATED_BODY()
public:
	void						SetEntity				(ISE_Abstract* InEntity) override;
	void						FillProperties			() override;
	void						PostEditChangeProperty	(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "Creature Abstract")
	float						Health;
	UPROPERTY(EditAnywhere, Category = "Creature Abstract")
	uint8						Team;
	UPROPERTY(EditAnywhere, Category = "Creature Abstract")
	uint8						Squad;
	UPROPERTY(EditAnywhere, Category = "Creature Abstract")
	uint8						Group;
protected:
	ISE_ALifeCreatureAbstract* ALifeCreatureAbstract = nullptr;
};


UCLASS()
class  UStalkerSpawnProperties_ALifeMonsterAbstract : public UStalkerSpawnProperties_ALifeCreatureAbstract
{
	GENERATED_BODY()
public:
	void						SetEntity(ISE_Abstract* InEntity) override;
	void						FillProperties() override;
	void						PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "ALife")
	bool						NoMoveInOffline;
	UPROPERTY(EditAnywhere, Category = "Monster Abstract")
	bool						UseSmartTerrainTasks;
	UPROPERTY(EditAnywhere, Category = "Monster Abstract")
	TArray<FString>				OutSpaceRestrictors;
	UPROPERTY(EditAnywhere, Category = "Monster Abstract")
	TArray<FString>				InSpaceRestrictors;
protected:
	ISE_ALifeMonsterAbstract*	ALifeMonsterAbstract = nullptr;
};


UCLASS()
class  UStalkerSpawnProperties_ALifeHumanAbstract : public UStalkerSpawnProperties_ALifeMonsterAbstract
{
	GENERATED_BODY()
public:
	void						SetEntity(ISE_Abstract* InEntity) override;
	void						FillProperties() override;
	void						PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "Human Abstract")
	bool						GroupBehaviour;
protected:
	ISE_ALifeHumanAbstract* ALifeHumanAbstract = nullptr;
};

UCLASS()
class  UStalkerSpawnProperties_ALifeMonsterZombie : public UStalkerSpawnProperties_ALifeMonsterAbstract
{
	GENERATED_BODY()
public:
	void						SetEntity(ISE_Abstract* InEntity) override;
	void						FillProperties() override;
	void						PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							EyeFov;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							EyeRange;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MinSpeed;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							AttackSpeed;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MaxPursuitRadius;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MaxHomeRadius;
	// attack

	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							HitPower;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	uint16							HitInterval;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							AttackDistance;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							AttackAngle;
protected:
	ISE_ALifeMonsterZombie* ALifeMonsterZombie = nullptr;
};

UCLASS()
class  UStalkerSpawnProperties_ALifeMonsterRat : public UStalkerSpawnProperties_ALifeMonsterAbstract
{
	GENERATED_BODY()
public:
	void							SetEntity				(ISE_Abstract* InEntity) override;
	void							FillProperties			() override;
	void							PostEditChangeProperty	(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							EyeFov;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							EyeRange;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MinSpeed;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							AttackSpeed;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MaxPursuitRadius;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MaxHomeRadius;
	// morale
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MoraleSuccessAttackQuant;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MoraleDeathQuant;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MoraleFearQuant;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MoraleRestoreQuant;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	uint16							MoraleRestoreTimeInterval;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MoraleMinValue;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MoraleMaxValue;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							MoraleNormalValue;
	// attack
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							HitPower;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	uint16							HitInterval;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							AttackDistance;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							AttackAngle;
	UPROPERTY(EditAnywhere, Category = "Monster Zombie")
	float							AttackSuccessProbability;
protected:
	ISE_ALifeMonsterRat* ALifeMonsterRat = nullptr;
};
