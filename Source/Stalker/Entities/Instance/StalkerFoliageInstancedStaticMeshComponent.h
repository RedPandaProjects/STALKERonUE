#pragma once

#include "Runtime/Foliage/Public/FoliageInstancedStaticMeshComponent.h"
#include "StalkerFoliageInstancedStaticMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class STALKER_API UStalkerFoliageInstancedStaticMeshComponent : public UFoliageInstancedStaticMeshComponent
{
	GENERATED_BODY()

public:
	virtual bool HasValidSettingsForStaticLighting(bool bOverlookInvalidComponents) const override;
};
