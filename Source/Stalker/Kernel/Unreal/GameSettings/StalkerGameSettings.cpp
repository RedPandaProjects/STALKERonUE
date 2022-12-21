#include "StalkerGameSettings.h"

UStalkerGameSettings::UStalkerGameSettings()
{
	EditorStartupGame = EStalkerGame::COP;
}

void UStalkerGameSettings::ReInitilizeXRay()
{
#if WITH_EDITOR
	if (FApp::IsGame())
	{
		return;
	}
    GXRayEngineManager->ReInitialized(EditorStartupGame);
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