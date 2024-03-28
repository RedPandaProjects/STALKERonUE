#include "StalkerGameSettings.h"
#include "Misc/MessageDialog.h"
#include "..\Stalker\Kernel\StalkerEngineManager.h"

int UStalkerGameSettings::GetActualGame()
{

	if (FStalkerEngineManager::IsFileExists("xrGameSOC"))
		return 0;
	else if (FStalkerEngineManager::IsFileExists("xrGame"))
		return 1;
	else if (FStalkerEngineManager::IsFileExists("xrGameCS"))
		return 2;
	else
		return 3;

}

UStalkerGameSettings::UStalkerGameSettings()
{
	switch (GetActualGame())
	{
	case 0:
	{
		EditorStartupGame = EStalkerGame::SHOC;
	}break;
	case 1:
	{
		EditorStartupGame = EStalkerGame::COP;
	}break;
	case 2:
	{
		EditorStartupGame = EStalkerGame::CS;
	}break;
	default:
		break;
	}
}
#if WITH_EDITOR
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
#endif
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