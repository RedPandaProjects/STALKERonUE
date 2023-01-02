#pragma once
#include "UI/Menu/StalkerMainMenu.h"
#include "UI/Menu/StalkerToolbarMenu.h"
#include "UI/Menu/StalkerBuildMenu.h"
class FStalkerEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void	OnPreExit();
	StalkerMainMenu MainMenu;
	StalkerToolbarMenu ToolBarMenu;
	StalkerBuildMenu	BuildMenu;
};
