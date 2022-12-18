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
	static void XRayConnect(const UObject* WorldContextObject, const FString& Server = TEXT("editor/single/alife/new"), const FString& Client = TEXT("localhost"));
	UFUNCTION(BlueprintCallable, Category = "XRay", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static void XRayShowMenu(const UObject* WorldContextObject, bool Show);
};
