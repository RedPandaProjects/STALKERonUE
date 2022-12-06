#pragma once
#include "StalkerKinematicsBoneData.generated.h"
UENUM()
enum class EStalkerShapeType:uint8
{
	EST_None,
	EST_Box,
	EST_Sphere,
	EST_Cylinder,
};

UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EStalkerShapeFlags
{
	ESF_NoPickable = (1 << 0), 	// use only in RayPick
	ESF_RemoveAfterBreak = (1 << 1),
	ESF_NoPhysics = (1 << 2),
	ESF_NoFogCollider = (1 << 3),
};
ENUM_CLASS_FLAGS(EStalkerShapeFlags);

USTRUCT()
struct STALKER_API FStalkerKinematicsBoneShape
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	EStalkerShapeType	Type = EStalkerShapeType::EST_None;
	UPROPERTY(EditAnywhere, Meta = (Bitmask, BitmaskEnum = "/Script/Stalker.EStalkerShapeFlags"))
	int32				Flags;
	UPROPERTY(EditAnywhere)
	FKBoxElem			Obb;
	UPROPERTY(EditAnywhere)
	FKSphereElem		Sphere;
	UPROPERTY(EditAnywhere)
	FKSphylElem			Cylinder;

};
UENUM()
enum class EStalkerJointType :uint8
{
	EJT_Rigid,
	EJT_Cloth,
	EJT_Joint,
	EJT_Wheel,
	EJT_None,
	EJT_Slider,
};

USTRUCT()
struct STALKER_API FStalkerKinematicsBoneIKData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	EStalkerJointType	Type = EStalkerJointType::EJT_Rigid;
	// by [axis XYZ on joint] and[Z-wheel,X-steer on wheel]

	UPROPERTY(EditAnywhere)
	FVector4f			LimitsX = FVector4f(0,0,1,1);
	UPROPERTY(EditAnywhere)
	FVector4f			LimitsY = FVector4f(0, 0, 1, 1);
	UPROPERTY(EditAnywhere)
	FVector4f			LimitsZ = FVector4f(0, 0, 1, 1);

	UPROPERTY(EditAnywhere)
	float				SpringFactor = 1;

	UPROPERTY(EditAnywhere)
	float				DampingFactor = 1;

	UPROPERTY(EditAnywhere)
	bool				IsBreakable = false;

	UPROPERTY(EditAnywhere)
	float				BreakForce = 0;	// [0..+INF]

	UPROPERTY(EditAnywhere)
	float				BreakTorque = 0;	// [0..+INF]

	UPROPERTY(EditAnywhere)
	float				Friction = 0;
};
USTRUCT()
struct STALKER_API FStalkerKinematicsBone
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	FName	PhysicsMaterialName = TEXT("default");
	UPROPERTY(EditAnywhere)
	float Mass = 1;
	UPROPERTY(EditAnywhere)
	FVector3f CenterOfMass;	
	UPROPERTY(EditAnywhere)
	FKBoxElem Obb = FKBoxElem(1);
	UPROPERTY(EditAnywhere)
	FStalkerKinematicsBoneShape Shape;
	UPROPERTY(EditAnywhere)
	FStalkerKinematicsBoneIKData IKData;

	void BuildFromLegacy(const  CBoneData&Legacy);
	void Build(class StalkerKinematicsBone&Bone);

};
