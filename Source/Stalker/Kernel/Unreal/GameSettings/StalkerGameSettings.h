#pragma once
#include "../../StalkerEngineManager.h"
#include "StalkerGameSettings.generated.h"

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Stalker Game Settings")) 
class STALKER_API UStalkerGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Editor")
	EStalkerGame EditorStartupGame;

	UStalkerGameSettings();

	
	void ReInitilizeXRay();
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

};