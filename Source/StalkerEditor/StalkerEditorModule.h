#pragma once
#include "UI/Menu/StalkerMainMenu.h"
#include "UI/Menu/StalkerToolbarMenu.h"
#include "UI/Menu/StalkerBuildMenu.h"
#include "UI/Menu/StalkerPlayMenu.h"
class FStalkerEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule	() override;
	virtual void ShutdownModule	() override;
	
private:
	void		OnPostEngineInit();
	void		OnPreExit		();
	StalkerMainMenu MainMenu;
	StalkerToolbarMenu ToolBarMenu;
	StalkerBuildMenu	BuildMenu;
	StalkerPlayMenu		PlayMenu;
};
