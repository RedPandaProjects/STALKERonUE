#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XrGameMaterialLibraryInterface.h"
THIRD_PARTY_INCLUDES_END
#include "StalkerPhysicalMaterial.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EStalkerPhysicalMaterialFlags1 : uint8
{
	NONE = 0 UMETA(Hidden),
	Breakable = (1ul << 0ul),
	Bounceable = (1ul << 2ul),
	Skidmark = (1ul << 3ul),
	Bloodmark = (1ul << 4ul),
	Climable = (1ul << 5ul),
	Passable = (1ul << 7ul),
};
ENUM_CLASS_FLAGS(EStalkerPhysicalMaterialFlags1);

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EStalkerPhysicalMaterialFlags2 : uint8
{
	NONE = 0 UMETA(Hidden),
	Dynamic = (1ul << 0ul),
	Liquid = (1ul << 1ul),
	SuppressShadows = (1ul << 2ul),
	SuppressWallmarks = (1ul << 3ul),
	ActorObstacle = (1ul << 4ul),
	NoRicoshet = (1ul << 5ul),
	Injurious = (1ul << 6ul),
	Shootable = (1ul << 7ul),
};
ENUM_CLASS_FLAGS(EStalkerPhysicalMaterialFlags2);
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EStalkerPhysicalMaterialFlags3 : uint8
{
	NONE = 0 UMETA(Hidden),
	Transparent = (1ul << 0ul),
	SlowDown = (1ul << 1ul)
};
ENUM_CLASS_FLAGS(EStalkerPhysicalMaterialFlags3);

UCLASS()
class STALKER_API UStalkerPhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float				PHFriction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float				PHDamping;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float				PHSpring;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float				PHBounceStartVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float				PHBouncing;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float		    	FlotationFactor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float				ShootFactor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float				ShootFactorMP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float				BounceDamageFactor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float				InjuriousSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float				VisTransparencyFactor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float				SndOcclusionFactor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float				DensityFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/Stalker.EStalkerPhysicalMaterialFlags1"))
	int32				Flags1 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/Stalker.EStalkerPhysicalMaterialFlags2"))
	int32				Flags2 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/Stalker.EStalkerPhysicalMaterialFlags3"))
	int32				Flags3 = 0;

	void BuildFromLegacy	(const SGameMtl&Legacy);
	void BuildToLegacy		(SGameMtl& Legacy);
};
