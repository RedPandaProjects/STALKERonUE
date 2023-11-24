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
	UStalkerGameSettings();

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Editor")
	EStalkerGame EditorStartupGame;


#if WITH_EDITORONLY_DATA
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Game",meta = (DisplayName = "Levels Of Shadow of Chernobyl"))
	TMap<FName, FStalkerLevelInfo> LevelsSOC;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Game", meta = (DisplayName = "Levels Of Clear sky"))
	TMap<FName, FStalkerLevelInfo> LevelsCS;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Game", meta = (DisplayName = "Levels Of Call of Pripyat"))
	TMap<FName, FStalkerLevelInfo> LevelsCOP;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Play In Editor")
	bool	NeedAutoBuildCForm		= true;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Play In Editor")
	bool	NeedAutoBuildAIMap		= true;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Play In Editor")
	bool	NeedAutoBuildLevelSpawn = true;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Play In Editor")
	bool	NeedAutoBuildGameSpawn	= true;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Play In Editor")
	bool	UseCurrentWorldSpawn	= true;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Play In Editor")
	bool	IgnoreIncludeInBuildSpawn = false;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Play In Editor")
	bool	VerifySpaceRestrictorBorders = true;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Editor")
	TArray<FString> ParticlesUsingHudMode; 
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Editor")
	TMap<FString,FString> ReplaceShaderWhenImport;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Editor")
	TSet<FString> DisableDistanceFieldOfCompilerShaderWhenImport; 

#endif
#if WITH_EDITOR
	const TMap<FName, FStalkerLevelInfo> & GetCurrentLevels() const;
#endif
	void ReInitilizeXRay();
#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};