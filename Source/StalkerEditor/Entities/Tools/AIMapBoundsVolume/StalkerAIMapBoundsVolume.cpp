#include "StalkerAIMapBoundsVolume.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(StalkerAIMapBoundsVolume)
AStalkerAIMapBoundsVolume::AStalkerAIMapBoundsVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GetBrushComponent()->Mobility = EComponentMobility::Static;
	BrushColor = FColor(200, 200, 200, 255);

	bColored = true;
	bIsEditorOnlyActor = true;
#if WITH_EDITORONLY_DATA
	bIsSpatiallyLoaded = false;
#endif
}