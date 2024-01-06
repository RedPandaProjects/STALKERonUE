#pragma once
#include "StalkerEditorManager.generated.h"

UCLASS()
class UStalkerEditorManager : public UObject
{
	GENERATED_BODY()
public:
	void											Initialized					();
	void											Destroy						();
	FString											GetGamePath					();
	void											ReloadConfigs				();
	void											ImportUITextures			();
	void											ImportAllTextures			();
	void											ImportWMTextures			();
	void											ImportGradTextures			();
	void											ImportMeshes				();
	void											ImportSounds				();
	void											ImportPhysicalMaterials		();
	void											ImportParticles				();

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
	void											OnPrePIEEnded				(const bool);
	void											OnReInitialized				();
	void											ScanSkeletons				();
	void											OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues);

};

extern UStalkerEditorManager*GStalkerEditorManager;