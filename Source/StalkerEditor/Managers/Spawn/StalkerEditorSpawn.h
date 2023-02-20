#pragma once
#include "StalkerEditorSpawn.generated.h"
UCLASS()
class UStalkerEditorSpawn : public UObject
{
	GENERATED_BODY()
public:
	void						Initialize					();
	void						Destroy						();
	class UStalkerLevelSpawn*	BuildLevelSpawnIfNeeded		();
	class UStalkerLevelSpawn*	BuildLevelSpawn				();
	bool						BuildGameSpawn				(class UStalkerLevelSpawn* OnlyIt = nullptr,bool IfNeededRebuild = false, bool IgnoreIncludeInBuild = false);
	void						BuildGameGraph				(class UStalkerGameSpawn* GameSpawn);
private:
	void						OnBuildLevelSpawn			();
	void						OnBuildGameSpawn			();
	void						OnPostWorldInitialization	(UWorld* World, const UWorld::InitializationValues IVS);
	UPROPERTY()
	TArray<class UStalkerLevelSpawn*>LevelSpawns;
};
