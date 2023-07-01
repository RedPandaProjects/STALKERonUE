#pragma once
#include "StalkerParticleDomain.generated.h"

UENUM(BlueprintType)
enum class EStalkerParticleDomainType:uint8
{
	Point 		= 0,	// Single point
	Line 		= 1,	// Line segment
	Triangle 	= 2,	// Triangle
	Plane 		= 3,	// Arbitrarily-oriented plane
	Box 		= 4,	// Axis-aligned box
	Sphere 		= 5,	// Sphere
	Cylinder 	= 6,	// Cylinder
	Cone 		= 7,	// Cone
	Blob 		= 8,	// Gaussian blob
	Disc 		= 9,	// Arbitrarily-oriented disc
	Rectangle	= 10,	// Rhombus-shaped planar region
};

USTRUCT()
struct STALKER_API FStalkerParticleDomain
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FVector3f					Position1;
	UPROPERTY(EditAnywhere)
	FVector3f					Position2;
	UPROPERTY(EditAnywhere)
	FVector3f					U;
	UPROPERTY(EditAnywhere)
	FVector3f					V;
	UPROPERTY(EditAnywhere)
	float						Radius1;
	UPROPERTY(EditAnywhere)
	float						Radius2;

	FStalkerParticleDomain():Position1(FVector3f::ZeroVector),Position2(FVector3f::ZeroVector),U(FVector3f::ZeroVector),V(FVector3f::ZeroVector),Radius1(0),Radius2(0)
	{

	}
};
