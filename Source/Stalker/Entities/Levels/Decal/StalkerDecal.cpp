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
	Decal->DecalSize = FVector{Size};
	// TODO: set rotation according to normal
}

// Called when the game starts or when spawned
void AStalkerDecal::BeginPlay()
{
	Super::BeginPlay();

	Decal->SetFadeOut(300.0f, 10.0f, true); // destroy after 5 minutes
}
