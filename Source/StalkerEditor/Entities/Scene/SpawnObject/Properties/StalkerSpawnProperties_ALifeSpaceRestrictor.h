#pragma once
#include "StalkerSpawnProperties_ALifeObject.h"
#include "StalkerSpawnProperties_ALifeSpaceRestrictor.generated.h"

UENUM()
enum class EStalkerDefaulRetrictorTypes
{
	NOTARestrictor UMETA(DisplayName = "NOT A Restrictor"),
	NONEDefaultRestrictor  UMETA(DisplayName = "NONE Default Restrictor"),
	OUTDefaultRestrictor UMETA(DisplayName = "OUT Default Restrictor"),
	INDefaultRestrictor UMETA(DisplayName = "IN Default Restrictor"),

};

UCLASS()
class  UStalkerSpawnProperties_ALifeSpaceRestrictor : public UStalkerSpawnProperties_ALifeObject
{
	GENERATED_BODY()
public:
	void				SetEntity				(ISE_Abstract* InEntity) override;
	void				FillProperties			() override;
	void				PostEditChangeProperty	(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere,Category = "SpaceRestrictor")
	bool				CheckForSeparator;
	UPROPERTY(EditAnywhere, Category = "SpaceRestrictor")
	EStalkerDefaulRetrictorTypes RetrictorType;
protected:
	ISE_ALifeSpaceRestrictor* ALifeSpaceRestrictor;
};

UCLASS()
class  UStalkerSpawnProperties_ALifeLevelChanger : public UStalkerSpawnProperties_ALifeSpaceRestrictor
{
	GENERATED_BODY()
public:
	void				SetEntity(ISE_Abstract* InEntity) override;
	void				FillProperties() override;
	void				PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "Level Changer", meta = (AllowedClasses = "/Script/Engine.World"))
	FSoftObjectPath		Map;

	UPROPERTY(EditAnywhere, Category = "Level Changer")
	FString				PointName;

	UPROPERTY(EditAnywhere, Category = "Level Changer")
	bool				SilentMode;
protected:
	void				SetMap();
	ISE_ALifeLevelChanger*ALifeLevelChanger;
};

UCLASS()
class  UStalkerSpawnProperties_ALifeCustomZone : public UStalkerSpawnProperties_ALifeSpaceRestrictor
{
	GENERATED_BODY()
public:
	void				SetEntity(ISE_Abstract* InEntity) override;
	void				FillProperties() override;
	void				PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, Category = "Custom Zone", meta = (DisplayName = "Shift Time (sec)"))
	uint32				ShiftTime;

	UPROPERTY(EditAnywhere, Category = "Custom Zone", meta = (DisplayName = "Enabled Time (sec)"))
	uint32				EnabledTime;

	UPROPERTY(EditAnywhere, Category = "Custom Zone", meta = (DisplayName = "Disabled Time (sec)"))
	uint32				DisabledTime;
protected:
	ISE_ALifeCustomZone* ALifeCustomZone;
};

UCLASS()
class  UStalkerSpawnProperties_ALifeAnomalousZone : public UStalkerSpawnProperties_ALifeCustomZone
{
	GENERATED_BODY()
public:
	void	SetEntity				(ISE_Abstract* InEntity) override;
	void	FillProperties			() override;
	void	PostEditChangeProperty	(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UPROPERTY(EditAnywhere, Category = "Anomalous Zone")
	float	OfflineInteractiveRadius;
	UPROPERTY(EditAnywhere, Category = "Anomalous Zone")
	uint32	ArtefactSpawnCount;
	UPROPERTY(EditAnywhere, Category = "Anomalous Zone")
	bool	VisibleForAI;
protected:
	ISE_ALifeAnomalousZone* ALifeAnomalousZone;
};

UCLASS()
class  UStalkerSpawnProperties_ALifeZoneVisual : public UStalkerSpawnProperties_ALifeAnomalousZone
{
	GENERATED_BODY()
public:
	void	SetEntity				(ISE_Abstract* InEntity) override;
	void	FillProperties			() override;
	void	PostEditChangeProperty	(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UPROPERTY(EditAnywhere, Category = "Zone Visual")
	class UAnimSequence* AttackAnimation;

protected:
	void		SetAnim();
	ISE_ALifeZoneVisual* ALifeZoneVisual;
};