#pragma once
#include "XRayObjectImportOptions.h"
#include "XRayLevelImportOptions.generated.h"

UCLASS(Config = EditorPerProjectUserSettings, HideCategories = (DebugProperty))
class UXRayLevelImportOptions : public UXRayObjectImportOptions
{
	GENERATED_BODY()
public:
	UXRayLevelImportOptions()
	{
		ImportStaticMeshes = true;
		ImportSpawnObjects = true;
		ImportWayObjects = true;
		ImportAIMap = true;
	}

	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level")
	bool						ImportStaticMeshes;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level")
	bool						ImportSpawnObjects;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level")
	bool						ImportWayObjects;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level")
	bool						ImportAIMap;
	
};
