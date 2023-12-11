#pragma once
#include "RBMKObjectImportOptions.generated.h"

UENUM(BlueprintType)
enum class  EXRayObjectImportGameFormat : uint8
{
	AUTO,
	SOC,
	CS_COP UMETA(DisplayName = "CS/COP"),
};
UCLASS(Config = EditorPerProjectUserSettings, HideCategories = (DebugProperty))
class URBMKObjectImportOptions : public UObject
{
	GENERATED_BODY()
public:
	URBMKObjectImportOptions()
	{
		ObjectImportGameFormat = EXRayObjectImportGameFormat::AUTO;
		AngleNormalSmoth = 75;
		DivideSubObject = false;
	}
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere,Category = "Object")
	EXRayObjectImportGameFormat ObjectImportGameFormat;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Object",meta = (DisplayName="Angle Normal Smoth(for soc)"))
	float						AngleNormalSmoth;
	UPROPERTY(BlueprintReadWrite, Config, EditAnywhere, Category = "Object",meta = (DisplayName="DivideSubObject(not support for level import)"))
	bool						DivideSubObject;
	
};
