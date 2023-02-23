#pragma once
#include "Importer/XRaySOCMaterials.h"
#include "StalkerEditorManager.generated.h"

UCLASS()
class UStalkerEditorManager : public UObject
{
	GENERATED_BODY()
public:
	void											Initialized					();
	void											Destroy						();
	inline const XRaySOCMaterials&					GetSOCMaterials				() const { return SOCMaterials; }
	FString											GetGamePath					();
	void											ReloadConfigs				();

	UPROPERTY(Transient)
	TArray< USkeleton*>								Skeletons;
	UPROPERTY(Transient)
	class UStalkerEditorCForm*						EditorCFrom;
	UPROPERTY(Transient)
	class UStalkerEditorAIMap*						EditorAIMap;
	UPROPERTY(Transient)
	class UStalkerSEFactoryManager*					SEFactoryManager;
	UPROPERTY(Transient)
	class UStalkerEditorSpawn*						EditorSpawn;


	TSharedPtr< FUICommandList>						UICommandList;
private:
	void											OnPreBeginPIE				(const bool);
	void											OnPostPIEStarted			(const bool);
	void											OnEndPIE					(const bool);
	void											OnReInitialized				();
	void											ScanSkeletons				();
	void											OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues);
	XRaySOCMaterials								SOCMaterials;

};

extern UStalkerEditorManager*GStalkerEditorManager;