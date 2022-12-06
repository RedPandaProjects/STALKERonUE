#include "StalkerKinematicsLegacy.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/IRenderable.h"
#include "XrCDB/ISpatial.h"
THIRD_PARTY_INCLUDES_END
#include "Kernel/XRay/Render/Resources/Visual/XRayKinematicsLegacy.h"
// Sets default values
AStalkerKinematicsLegacy::AStalkerKinematicsLegacy()
{
	Kinematics = nullptr;
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SceneComponent->SetupAttachment(GetRootComponent());
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(SceneComponent);

	SetHidden(true);
}

void AStalkerKinematicsLegacy::SetKinematics(XRayKinematicsLegacy* InKinematics)
{
	check(InKinematics&& Kinematics == nullptr);
	Kinematics = InKinematics; 
	Fvector3 InCenter;
	InKinematics->VisData.box.getcenter(InCenter);
	Fvector3 InSize;
	InKinematics->VisData.box.getsize(InSize);
	FVector Center(( - InCenter.x * 100 ), (InCenter.z * 100 ), (InCenter.y * 100 ));
	MeshComponent->SetRelativeLocation(Center);
	FVector Size(InSize.x, InSize.z, InSize.y);
	MeshComponent->SetRelativeScale3D(Size);
}

// Called every frame
void AStalkerKinematicsLegacy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Kinematics->Renderable && Kinematics->Renderable->MySpatial)
		SetActorHiddenInGame(!(Kinematics->Renderable->MySpatial->spatial.type & STYPE_RENDERABLE));
	else
		SetActorHiddenInGame(true);
	if (Kinematics&& Kinematics->Renderable)
	{
		Kinematics->CalculateBones(true);
		Fquaternion XRayQuat;
		XRayQuat.set(Kinematics->Renderable->renderable.xform);
		FQuat Quat(XRayQuat.x, -XRayQuat.z, -XRayQuat.y, XRayQuat.w);
		Fvector InLocation = Kinematics->Renderable->renderable.xform.c;
		FVector Location(-InLocation.x * 100, InLocation.z * 100, InLocation.y * 100);
		SetActorLocationAndRotation(Location, Quat);
	}
}

void AStalkerKinematicsLegacy::PostInitProperties()
{
	Super::PostInitProperties();
	if (HasAnyFlags(RF_NeedLoad | RF_WasLoaded | RF_ClassDefaultObject) == false)
	{
		MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(this, TEXT("/Engine/BasicShapes/Cube.Cube")));
	
	}
}

