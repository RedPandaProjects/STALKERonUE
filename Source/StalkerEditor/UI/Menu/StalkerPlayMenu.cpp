#include "StalkerPlayMenu.h"
#include "../Commands/StalkerEditorCommands.h"
#include "../StalkerEditorStyle.h"
#include "../../StalkerEditorManager.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"

void FStalkerPlayMenu::Initialize()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	if (UToolMenu* ProfileMenu = UToolMenus::Get()->ExtendMenu(TEXT("UnrealEd.PlayWorldCommands.PlayMenu")))
	{
		FToolMenuSection& Section = ProfileMenu->AddSection("Stalker", FText::FromString(TEXT("Stalker")));
		Section.AddSubMenu(
			"Auto build",
			FText::FromString(TEXT("Auto build")),
			FText::FromString(TEXT("Auto build")),
			FNewToolMenuDelegate::CreateRaw(this, &FStalkerPlayMenu::CreateAutoBuildSubMenu)
		);

	
		{
			FUIAction Action(FExecuteAction::CreateLambda(
			[]()
			{
				UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
				SGSettings->UseCurrentWorldSpawn = true; 
				SGSettings->PostEditChange(); 
				SGSettings->SaveConfig(); 
			}),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([]() 
			{
				UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
				return SGSettings->UseCurrentWorldSpawn;
			}));
			Section.AddMenuEntry("BuildCurrentWorldGameSpawn",FText::FromString(TEXT("Build current world spawn")), FText::FromString(TEXT("Build current world spawn")), FSlateIcon(), Action, EUserInterfaceActionType::RadioButton);
		}
		{
			FUIAction Action(FExecuteAction::CreateLambda(
			[]()
			{
				UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
				SGSettings->UseCurrentWorldSpawn = false; 
				SGSettings->PostEditChange(); 
				SGSettings->SaveConfig(); 
			}),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([]() 
			{
				UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
				return !SGSettings->UseCurrentWorldSpawn;
			}));
			Section.AddMenuEntry("BuildFullGameSpawn",FText::FromString(TEXT("Build full spawn")), FText::FromString(TEXT("Build full spawn")), FSlateIcon(), Action, EUserInterfaceActionType::RadioButton);
		}
		Section.AddSeparator(NAME_None);
		{
			FUIAction Action(FExecuteAction::CreateLambda(
			[]()
			{
					UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
					SGSettings->IgnoreIncludeInBuildSpawn = !SGSettings->IgnoreIncludeInBuildSpawn;
					SGSettings->PostEditChange();
					SGSettings->SaveConfig();
			}),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([]()
			{
				UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
				return SGSettings->IgnoreIncludeInBuildSpawn;
			}));
			Section.AddMenuEntry("IgnoreIncludeInBuildSpawn", FText::FromString(TEXT("Ignore \"Include In Build Spawn\"")), FText::FromString(TEXT("Ignore \"Include In Build Spawn\"")), FSlateIcon(), Action, EUserInterfaceActionType::Check);
		}
		{
			FUIAction Action(FExecuteAction::CreateLambda(
			[]()
			{
					UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
					SGSettings->VerifySpaceRestrictorBorders = !SGSettings->VerifySpaceRestrictorBorders;
					SGSettings->PostEditChange();
					SGSettings->SaveConfig();
			}),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([]()
			{
				UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
				return SGSettings->VerifySpaceRestrictorBorders;
			}));
			Section.AddMenuEntry("VerifySpaceRestrictorBorders", FText::FromString(TEXT("Verify Space Restrictor Borders")), FText::FromString(TEXT("Verify Space Restrictor Borders")), FSlateIcon(), Action, EUserInterfaceActionType::Check);
		}

	}
	else
	{
		UE_LOG(LogStalkerEditor, Error, TEXT("Can't find 'UnrealEd.PlayWorldCommands.PlayMenu' menu section"))
	}
}

void FStalkerPlayMenu::Destroy()
{

}

void FStalkerPlayMenu::CreateAutoBuildSubMenu(UToolMenu* InToolMenu)
{


	FToolMenuSection& Section = InToolMenu->AddSection("Section");
	{
		FUIAction Action(FExecuteAction::CreateLambda(
		[]()
		{
			UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
			SGSettings->NeedAutoBuildCForm = !SGSettings->NeedAutoBuildCForm; 
			SGSettings->PostEditChange(); 
			SGSettings->SaveConfig(); 
		}),
		FCanExecuteAction(),
		FIsActionChecked::CreateLambda([]() 
		{
			UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
			return SGSettings->NeedAutoBuildCForm; 
		}));
		Section.AddMenuEntry("BuildCFrom",		 FText::FromString(TEXT("Auto build CFrom")), FText::FromString(TEXT("Auto build CFrom")), FSlateIcon(), Action, EUserInterfaceActionType::Check);
	}
	{
		FUIAction Action(FExecuteAction::CreateLambda(
		[]()
		{
			UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
			SGSettings->NeedAutoBuildAIMap = !SGSettings->NeedAutoBuildAIMap;
			SGSettings->PostEditChange(); 
			SGSettings->SaveConfig(); 
		}),
		FCanExecuteAction(),
		FIsActionChecked::CreateLambda([]() 
		{
			UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
			return SGSettings->NeedAutoBuildAIMap;
		}));
		Section.AddMenuEntry("BuildAIMap", FText::FromString(TEXT("Auto build AIMap")), FText::FromString(TEXT("Auto build AIMap")), FSlateIcon(), Action, EUserInterfaceActionType::Check);
	}
	{
		
		FUIAction Action(FExecuteAction::CreateLambda(
		[]()
		{
			UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
			SGSettings->NeedAutoBuildLevelSpawn = !SGSettings->NeedAutoBuildLevelSpawn;
			SGSettings->PostEditChange(); 
			SGSettings->SaveConfig(); 
		}),
		FCanExecuteAction(),
		FIsActionChecked::CreateLambda([]() 
		{
			UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
			return SGSettings->NeedAutoBuildLevelSpawn;
		}));
		Section.AddMenuEntry("BuildLevelSpawn", FText::FromString(TEXT("Auto build LevelSpawn")), FText::FromString(TEXT("Auto build LevelSpawn")), FSlateIcon(), Action, EUserInterfaceActionType::Check);
	}
	{
		FUIAction Action(FExecuteAction::CreateLambda(
		[]()
		{
			UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
			SGSettings->NeedAutoBuildGameSpawn = !SGSettings->NeedAutoBuildGameSpawn;
			SGSettings->PostEditChange(); 
			SGSettings->SaveConfig(); 
		}),
		FCanExecuteAction(),
		FIsActionChecked::CreateLambda([]() 
		{
			UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
			return SGSettings->NeedAutoBuildGameSpawn;
		}));
		Section.AddMenuEntry("BuildGameSPawn", FText::FromString(TEXT("Auto build GameSpawn")), FText::FromString(TEXT("Auto build GameSpawn")), FSlateIcon(), Action, EUserInterfaceActionType::Check);
	}

}

