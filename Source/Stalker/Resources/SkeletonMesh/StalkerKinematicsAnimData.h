#pragma once
#include "StalkerKinematicsAnimData.generated.h"

UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EStalkerKinematicsAnimFlags
{
	FX = 1 << 0,
	StopAtEnd = 1 << 1,
	SyncPart = 1 << 3,
	UseFootSteps = 1 << 4,
	RootMover = 1 << 5,
	Idle = 1 << 6,
};
ENUM_CLASS_FLAGS(EStalkerKinematicsAnimFlags)


USTRUCT()
struct STALKER_API FStalkerKinematicsAnimMarkInterval
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Start;
	UPROPERTY(EditAnywhere)
	float End;
};

USTRUCT()
struct STALKER_API FStalkerKinematicsAnimMark
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FStalkerKinematicsAnimMarkInterval> Intervals;
};

USTRUCT()
struct STALKER_API FStalkerKinematicsAnimData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	UAnimSequence*			Amim;
	UPROPERTY(EditAnywhere)
	FName					FXBoneOrPart;
	
	UPROPERTY(EditAnywhere)
	float					Power;

	UPROPERTY(EditAnywhere)
	float					Accrue;

	UPROPERTY(EditAnywhere)
	float					Falloff;
	UPROPERTY(EditAnywhere)
	TArray< FStalkerKinematicsAnimMark>	Marks;
	UPROPERTY(EditAnywhere,Meta = (Bitmask, BitmaskEnum = "/Script/Stalker.EStalkerKinematicsAnimFlags"))
	int32					Flags;

	void BuildFromLegacy(CMotionDef&Def,TArray<TSharedPtr<CBoneData>>&Bones, CPartition& Partition);
};
