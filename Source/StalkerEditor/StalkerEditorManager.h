#pragma once
#include "Importer/XRaySOCMaterials.h"
#include "StalkerEditorManager.generated.h"
UCLASS()
class UStalkerEditorManager : public UObject
{
	GENERATED_BODY()
public:
	void											Initialized		();
	void											Destroy			();
	inline const XRaySOCMaterials&					GetSOCMaterials	() const { return SOCMaterials; }
	FString											GetGamePath		();
	UPROPERTY(Transient)
	TArray< USkeleton*>								Skeletons;
private:
	void											OnReInitialized	();
	void											ScanSkeletons	();
	FDelegateHandle									DelegateHandleOnReInitialized;
	XRaySOCMaterials								SOCMaterials;

};

extern UStalkerEditorManager*GStalkerEditorManager;