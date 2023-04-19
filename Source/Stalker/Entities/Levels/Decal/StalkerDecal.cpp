#include "StalkerDecal.h"

// Sets default values
AStalkerDecal::AStalkerDecal()
{
	PrimaryActorTick.bCanEverTick = false;

	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	SetRootComponent(Decal);
}

void AStalkerDecal::SetDecalData
	(const FVector& Location, float Size, const FVector& Normal, UMaterialInstanceDynamic* Material)
{
	Decal->SetDecalMaterial(Material);
	Decal->SetRelativeLocation(Location);
	FRotator Rotator = FRotationMatrix::MakeFromX(Normal).Rotator();
	Rotator.Roll += 90.f + FMath::FRandRange(-20.0f, 20.0f);
	Decal->SetRelativeRotation(Rotator);
	Decal->DecalSize = FVector{Size};
}

// Called when the game starts or when spawned
void AStalkerDecal::BeginPlay()
{
	Super::BeginPlay();

	Decal->SetFadeOut(300.0f, 10.0f, true); // destroy after 5 minutes
}
