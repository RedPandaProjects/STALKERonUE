#include "StalkerMainMenu.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"
#include "Framework/Commands/Commands.h"
#include "../StalkerEditorStyle.h"

void StalkerMainMenu::Initialize()
{
	if (!IsRunningCommandlet())
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		auto LevelEditorMenuExtensibilityManager = LevelEditorModule.GetMenuExtensibilityManager();
		MenuExtender = MakeShareable(new FExtender);
		MenuExtender->AddMenuBarExtension("Window", EExtensionHook::After, nullptr, FMenuBarExtensionDelegate::CreateRaw(this, &StalkerMainMenu::MakePulldownMenu));
		LevelEditorMenuExtensibilityManager->AddExtender(MenuExtender);
	}
}

void StalkerMainMenu::Destroy()
{

}

void StalkerMainMenu::MakePulldownMenu(FMenuBarBuilder& InMenuBuilder)
{
	InMenuBuilder.AddPullDownMenu(
		FText::FromString("Stalker"),
		FText::FromString("Open the Stalker menu"),
		FNewMenuDelegate::CreateRaw(this, &StalkerMainMenu::FillPulldownMenu),
		"Stalker",
		"StalkerMenu"
	);
}

void StalkerMainMenu::FillPulldownMenu(FMenuBuilder& InMenuBuilder)
{
	InMenuBuilder.BeginSection(NAME_None, FText::FromString("Games"));

	{
		FUIAction Action(
			FExecuteAction::CreateLambda([]()
				{
					UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
					if (SGSettings->EditorStartupGame != EStalkerGame::COP)
					{
						SGSettings->EditorStartupGame = EStalkerGame::COP;
						SGSettings->TryUpdateDefaultConfigFile();
						SGSettings->ReInitilizeXRay();
					}
				}
			),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([]()
				{
					const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>();
					return SGSettings->EditorStartupGame == EStalkerGame::COP;
				}
			)
					);
		InMenuBuilder.AddMenuEntry(FText::FromString("Call of Pripyat"), FText::GetEmpty(), FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), "StalkerEditor.COP"), Action, NAME_None, EUserInterfaceActionType::RadioButton);
	}
	{
		FUIAction Action(
			FExecuteAction::CreateLambda([]()
				{
					UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
					if (SGSettings->EditorStartupGame != EStalkerGame::CS)
					{
						SGSettings->EditorStartupGame = EStalkerGame::CS;
						SGSettings->TryUpdateDefaultConfigFile();
						SGSettings->ReInitilizeXRay();
					}
				}
			),
			FCanExecuteAction(),
					FIsActionChecked::CreateLambda([]()
						{
							const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>();
							return SGSettings->EditorStartupGame == EStalkerGame::CS;
						}
					)
					);
		InMenuBuilder.AddMenuEntry(FText::FromString("Clear Sky"), FText::GetEmpty(), FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), "StalkerEditor.CS"), Action, NAME_None, EUserInterfaceActionType::RadioButton);
	}
	{
		FUIAction Action(
			FExecuteAction::CreateLambda([]()
				{
					UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
					if (SGSettings->EditorStartupGame != EStalkerGame::SHOC)
					{
						SGSettings->EditorStartupGame = EStalkerGame::SHOC;
						SGSettings->TryUpdateDefaultConfigFile();
						SGSettings->ReInitilizeXRay();
					}
				}
			),
			FCanExecuteAction(),
					FIsActionChecked::CreateLambda([]()
						{
							const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>();
							return SGSettings->EditorStartupGame == EStalkerGame::SHOC;
						}
					)
					);
		InMenuBuilder.AddMenuEntry(FText::FromString("Shadow of Chernobyl"), FText::GetEmpty(), FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), "StalkerEditor.SOC"), Action, NAME_None, EUserInterfaceActionType::RadioButton);
	}

	InMenuBuilder.EndSection();
}