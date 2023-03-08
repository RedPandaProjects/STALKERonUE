#include "StalkerGameSettings.h"

UStalkerGameSettings::UStalkerGameSettings()
{
	EditorStartupGame = EStalkerGame::COP;
}

const TMap<FName, FStalkerLevelInfo>& UStalkerGameSettings::GetCurrentLevels() const
{
	switch (GStalkerEngineManager->GetCurrentGame())
	{
	default:
		return LevelsCOP;
	case EStalkerGame::CS:
		return LevelsCS;
	case EStalkerGame::SHOC:
		return LevelsSOC;
	}
}

void UStalkerGameSettings::ReInitilizeXRay()
{
#if WITH_EDITOR
	if (FApp::IsGame())
	{
		return;
	}
    GStalkerEngineManager->ReInitialized(EditorStartupGame);
#endif
}

#if WITH_EDITOR
void UStalkerGameSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UStalkerGameSettings, EditorStartupGame))
	{
		ReInitilizeXRay();
	}


}

#endif