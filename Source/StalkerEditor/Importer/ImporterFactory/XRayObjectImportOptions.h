#pragma once
#include "XRayObjectImportOptions.generated.h"

UENUM(BlueprintType)
enum class  EXRayObjectImportGameFormat : uint8
{
	AUTO,
	SOC,
	CS_COP UMETA(DisplayName = "CS/COP"),
};
UCLASS(Config = EditorPerProjectUserSettings, HideCategories = (DebugProperty))
class UXRayObjectImportOptions : public UObject
{
	GENERATED_BODY()
public:
	UXRayObjectImportOptions()
	{
		ObjectImportGameFormat = EXRayObjectImportGameFormat::AUTO;
		AngleNormalSmoth = 75;
	}
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere,Category = "Object")
	EXRayObjectImportGameFormat ObjectImportGameFormat;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Object",meta = (DisplayName="Angle Normal Smoth(for soc)"))
	float						AngleNormalSmoth;
	
};
