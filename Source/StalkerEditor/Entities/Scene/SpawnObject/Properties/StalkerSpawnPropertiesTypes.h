#pragma once
#include "StalkerSpawnPropertiesTypes.generated.h"

USTRUCT(meta = (DisplayName = "Storys"))
struct FStalkerSpawnData_Storys
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FString		Value;
};

USTRUCT(meta = (DisplayName = "Spawn Storys"))
struct FStalkerSpawnData_SpawnStorys
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FString		Value;
};

USTRUCT(meta = (DisplayName = "Location 1"))
struct FStalkerSpawnData_Location1
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FString		Value;
};
USTRUCT(meta = (DisplayName = "Location 2"))
struct FStalkerSpawnData_Location2
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FString		Value;
};
USTRUCT(meta = (DisplayName = "Location 3"))
struct FStalkerSpawnData_Location3
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FString		Value;
};
USTRUCT(meta = (DisplayName = "Location 4"))
struct FStalkerSpawnData_Location4
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FString		Value;
};

USTRUCT(meta = (DisplayName = "Light Anim"))
struct FStalkerSpawnData_LightAnim
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FString		Value;
};

USTRUCT(meta = (DisplayName = "Character Profile"))
struct FStalkerSpawnData_CharacterProfile
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FString		Value;
};

USTRUCT(meta = (DisplayName = "Loophole"))
struct FStalkerSpawnData_Loophole
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(VisibleAnywhere)
	FString		Name;
	UPROPERTY(EditAnywhere)
	bool		Use;

	shared_str	XRayName;
};

USTRUCT(meta = (DisplayName = "Smart Cover Description"))
struct FStalkerSpawnData_SmartCoverDescription
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FString		Value;
};