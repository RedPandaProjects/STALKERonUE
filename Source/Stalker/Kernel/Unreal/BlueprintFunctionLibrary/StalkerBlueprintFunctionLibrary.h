// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StalkerBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STALKER_API UStalkerBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "XRay", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static void StalkerConnect(const UObject* WorldContextObject, const FString& Server = TEXT("editor/single/alife/new"), const FString& Client = TEXT("localhost"));
	UFUNCTION(BlueprintCallable, Category = "XRay", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static void StalkerShowMenu(const UObject* WorldContextObject, bool Show);
	UFUNCTION(BlueprintCallable, Category = "Stalker|Level Script",meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static void StalkerActorTransferInfo(const FString&Name,bool Value = true);
	UFUNCTION(BlueprintCallable, Category = "Stalker|Level Script", meta = (Latent, LatentInfo = "LatentInfo",WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static void StalkerActorWaitInfoList(const TMap<FString,bool>&CheckValues,const UObject* WorldContextObject, FLatentActionInfo LatentInfo);
	UFUNCTION(BlueprintCallable, Category = "Stalker|Level Script", meta = (Latent, LatentInfo = "LatentInfo",WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static void StalkerActorWaitInfo(const FString&Name,bool Value,const UObject* WorldContextObject, FLatentActionInfo LatentInfo);
	UFUNCTION(BlueprintCallable, Category = "Stalker|Level Script",meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static bool StalkerActorGetInfo(const FString&Name);
	UFUNCTION(BlueprintCallable, Category = "Stalker|Level Script",meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static void StalkerSpawnObject(const FString&SectionName,const FString&WayObjectName,int32 PointIndex = 0, float Angle = 0);
	UFUNCTION(BlueprintCallable, Category = "Stalker|Level Script",meta = ( UnsafeDuringActorConstruction = "true"))
	static float StalkerGetGameTime();
};
