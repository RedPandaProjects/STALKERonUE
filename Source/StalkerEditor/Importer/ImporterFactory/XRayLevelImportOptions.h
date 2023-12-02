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
		ImportParticles = true;
		ImportWallmark = true;
		ImportDetail = true;
	}

	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level")
	bool						ImportStaticMeshes;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level")
	bool						ImportSpawnObjects;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level")
	bool						ImportWayObjects;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level")
	bool						ImportAIMap;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level")
	bool						ImportParticles;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level")
	bool						ImportWallmark;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level")
	bool						ImportDetail;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level", meta=(ClampMin = 0.2, ClampMax = 1))
	float						DetailDensity		= 0.4f;
	
};
