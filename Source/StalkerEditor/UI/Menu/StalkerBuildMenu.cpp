#include "StalkerBuildMenu.h"
#include "../Commands/StalkerEditorCommands.h"
#include "../StalkerEditorStyle.h"
#include "../../StalkerEditorManager.h"

void FStalkerBuildMenu::Initialize()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	if (UToolMenu* ProfileMenu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Build")))
	{
		FToolMenuSection& Section = ProfileMenu->AddSection("Stalker Build", FText::FromString(TEXT("Stalker Build")));

		Section.AddMenuEntry(StalkerEditorCommands::Get().BuildCForm,
			FText::FromString(TEXT("Build CForm")),
			FText::FromString(TEXT("Build CForm")),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), TEXT("StalkerEditor.BuildCForm"))
		).SetCommandList(GStalkerEditorManager->UICommandList);

		Section.AddMenuEntry(StalkerEditorCommands::Get().BuildAIMap,
			FText::FromString(TEXT("Build AIMap")),
			FText::FromString(TEXT("Build AIMap")),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), TEXT("StalkerEditor.BuildAIMap"))
		).SetCommandList(GStalkerEditorManager->UICommandList);

		Section.AddMenuEntry(StalkerEditorCommands::Get().BuildLevelSpawn,
			FText::FromString(TEXT("Build LevelSpawn")),
			FText::FromString(TEXT("Build LevelSpawn")),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), TEXT("StalkerEditor.BuildLevelSpawn"))
		).SetCommandList(GStalkerEditorManager->UICommandList);

		Section.AddMenuEntry(StalkerEditorCommands::Get().BuildGameSpawn,
			FText::FromString(TEXT("Build GameSpawn")),
			FText::FromString(TEXT("Build GameSpawn")),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), TEXT("StalkerEditor.BuildGameSpawn"))
		).SetCommandList(GStalkerEditorManager->UICommandList);
	}
	else
	{
		UE_LOG(LogStalkerEditor, Error, TEXT("Can't find 'MainFrame.MainMenu.Build' menu section"))
	}
}

void FStalkerBuildMenu::Destroy()
{

}

