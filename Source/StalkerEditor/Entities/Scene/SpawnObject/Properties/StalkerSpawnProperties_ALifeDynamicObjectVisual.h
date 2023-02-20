#pragma once
#include "StalkerSpawnProperties_ALifeObject.h"
#include "StalkerSpawnProperties_ALifeDynamicObjectVisual.generated.h"

UENUM()
enum class EStalkerObjectPhysicTypes
{
	Box,
	FixedChain,
	FreeChain,
	Skeleton,

};

UCLASS()
class  UStalkerSpawnProperties_ALifeObjectPhysic : public UStalkerSpawnProperties_ALifeObject
{
	GENERATED_BODY()
public:
	void						SetEntity				(ISE_Abstract* InEntity) override;
	void						FillProperties			() override;
	void						PostEditChangeProperty	(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "ObjectPhysic",meta = (DisplayName="Type"))
	EStalkerObjectPhysicTypes	ObjectPhysicType;
	UPROPERTY(EditAnywhere, Category = "ObjectPhysic", meta = (DisplayName = "Mass"))
	float						ObjectPhysicMass;
	UPROPERTY(EditAnywhere, Category = "ObjectPhysic", meta = (DisplayName = "Active"))
	bool						ObjectPhysicActive;
	UPROPERTY(EditAnywhere, Category = "ObjectPhysic")
	TArray<FString>				FixedBones;
protected:
	ISE_ALifeObjectPhysic*		ALifeObjectPhysic = nullptr;
};


UCLASS()
class  UStalkerSpawnProperties_ALifeHelicopter : public UStalkerSpawnProperties_ALifeObject
{
	GENERATED_BODY()
public:
	void				SetEntity				(ISE_Abstract* InEntity) override;
	void				FillProperties			() override;
	void				PostEditChangeProperty	(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "Helicopter")
	FString				EngineSound;
protected:
	ISE_ALifeHelicopter*ALifeHelicopter = nullptr;
};

UCLASS()
class  UStalkerSpawnProperties_ALifeCar  : public UStalkerSpawnProperties_ALifeObject
{
	GENERATED_BODY()
public:
	void				SetEntity				(ISE_Abstract* InEntity) override;
	void				FillProperties			() override;
	void				PostEditChangeProperty	(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "Car")
	float				Health;
protected:
	ISE_ALifeCar*		ALifeCar = nullptr;
};

UCLASS()
class  UStalkerSpawnProperties_ALifeObjectBreakable : public UStalkerSpawnProperties_ALifeObject
{
	GENERATED_BODY()
public:
	void						SetEntity				(ISE_Abstract* InEntity) override;
	void						FillProperties			() override;
	void						PostEditChangeProperty	(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "ObjectBreakable")
	float						Health;
protected:
	ISE_ALifeObjectBreakable*	ALifeObjectBreakable = nullptr;
};

