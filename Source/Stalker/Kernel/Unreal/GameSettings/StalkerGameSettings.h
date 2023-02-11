#pragma once
#include "../../StalkerEngineManager.h"
#include "StalkerGameSettings.generated.h"



USTRUCT(BlueprintType)
struct FStalkerLevelInfo
{
	GENERATED_USTRUCT_BODY()


	UPROPERTY(config, EditAnywhere, BlueprintReadOnly,Category = "Base", meta = (AllowedClasses = "/Script/Engine.World"))
	FSoftObjectPath Map;

	UPROPERTY(config, EditAnywhere, Category = "Editor")
	bool			IncludeInBuildSpawn = true;
};



UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Stalker Game Settings")) 
class STALKER_API UStalkerGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Editor")
	EStalkerGame EditorStartupGame;

	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Game",meta = (DisplayName = "Levels Of Shadow of Chernobyl"))
	TMap<FName, FStalkerLevelInfo> LevelsSOC;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Game", meta = (DisplayName = "Levels Of Clear sky"))
	TMap<FName, FStalkerLevelInfo> LevelsCS;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Game", meta = (DisplayName = "Levels Of Call of Pripyat"))
	TMap<FName, FStalkerLevelInfo> LevelsCOP;


	UStalkerGameSettings();

	const TMap<FName, FStalkerLevelInfo> & GetCurrentLevels() const;

	void ReInitilizeXRay();
#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};