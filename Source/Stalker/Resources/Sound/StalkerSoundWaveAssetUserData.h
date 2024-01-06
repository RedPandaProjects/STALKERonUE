#pragma once
#include "StalkerSoundWaveAssetUserData.generated.h"

UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EStalkerSoundTypes:uint32
{
	NoSound					= 0x00000000 UMETA(Hidden),
	Weapon					= 0x80000000,
	Item					= 0x40000000,
	Monster					= 0x20000000,
	Anomaly					= 0x10000000,
	World					= 0x08000000,
	PickingUp				= 0x04000000,
	Dropping				= 0x02000000,
	Hiding					= 0x01000000,
	Taking					= 0x00800000,
	Using					= 0x00400000,
	Shooting				= 0x00200000,
	EmptyClicking			= 0x00100000,
	BulletHit				= 0x00080000,
	Recharging				= 0x00040000,
	Dying					= 0x00020000,
	Injuring				= 0x00010000,
	Step					= 0x00008000,
	Talking					= 0x00004000,
	Attacking				= 0x00002000,
	Eating					= 0x00001000,
	Idle					= 0x00000800,
	ObjectBreaking			= 0x00000400,
	ObjectColliding			= 0x00000200,
	ObjectExploding			= 0x00000100,
	Ambient					= 0x00000080,
	FromSource				= 0xFFFFFFFF UMETA(Hidden),
};
ENUM_CLASS_FLAGS(EStalkerSoundTypes);

UCLASS()
class STALKER_API UStalkerSoundWaveAssetUserData : public UAssetUserData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta=(ClampMin=0, ClampMax=1))
	float AIRatioToDistance = 1;

	UPROPERTY(EditAnywhere,Meta = (Bitmask, BitmaskEnum = "/Script/Stalker.EStalkerSoundTypes"))
	int32 Flags;
};
