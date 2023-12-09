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
		ImportDetails = true;
		ImportMultipleUsageMeshesAsFoliage = true;
	}

	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level|StaticMeshes")
	bool						ImportStaticMeshes;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level|StaticMeshes")
	bool						ImportMultipleUsageMeshesAsFoliage;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level|Logic")
	bool						ImportSpawnObjects;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level|Logic")
	bool						ImportWayObjects;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level|Logic")
	bool						ImportAIMap;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level|Environment")
	bool						ImportParticles;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level|Environment")
	bool						ImportWallmark;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level|Details")
	bool						ImportDetails;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Level|Details", meta=(ClampMin = 0.2, ClampMax = 1))
	float						DetailsDensity		= 0.4f;
	
};
