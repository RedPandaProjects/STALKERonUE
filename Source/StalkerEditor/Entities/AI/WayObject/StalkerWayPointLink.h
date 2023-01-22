#pragma once
#include "StalkerWayPointLink.generated.h"
USTRUCT()
struct FStalkerWayPointLink
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Link")
	float							Probability;
	UPROPERTY()
	class UStalkerWayPointComponent* Point;
};
