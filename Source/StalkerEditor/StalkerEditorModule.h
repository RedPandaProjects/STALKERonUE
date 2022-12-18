#pragma once
#include "UI/Menu/StalkerMainMenu.h"
class FStalkerEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	StalkerMainMenu MainMenu;
};
