#include "StalkerToolbarMenu.h"
#include "../StalkerEditorStyle.h"
#include "../Commands/StalkerEditorCommands.h"
#include "../../StalkerEditorManager.h"
void FStalkerToolbarMenu::Initialize()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	if (UToolMenu* ProfileMenu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.LevelEditorToolBar.User")))
	{
		FToolMenuSection& Section = ProfileMenu->AddSection("Stalker Tools", FText::FromString(TEXT("Stalker Tools")));
		
		Section.AddSeparator(NAME_None);
		
		Section.AddEntry(FToolMenuEntry::InitToolBarButton(StalkerEditorCommands::Get().BuildCForm,
			FText::FromString(TEXT("Build CForm")),
			FText::FromString(TEXT("Build CForm")),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), TEXT("StalkerEditor.BuildCForm"))
		)).SetCommandList(GStalkerEditorManager->UICommandList);
		Section.AddEntry(FToolMenuEntry::InitToolBarButton(StalkerEditorCommands::Get().BuildAIMap,
			FText::FromString(TEXT("Build AIMap")),
			FText::FromString(TEXT("Build AIMap")),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), TEXT("StalkerEditor.BuildAIMap"))
		)).SetCommandList(GStalkerEditorManager->UICommandList);
		Section.AddEntry(FToolMenuEntry::InitToolBarButton(StalkerEditorCommands::Get().BuildLevelSpawn,
			FText::FromString(TEXT("Build LevelSpawn")),
			FText::FromString(TEXT("Build LevelSpawn")),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), TEXT("StalkerEditor.BuildLevelSpawn"))
		)).SetCommandList(GStalkerEditorManager->UICommandList);
		Section.AddSeparator(NAME_None);
		Section.AddEntry(FToolMenuEntry::InitToolBarButton(StalkerEditorCommands::Get().BuildGameSpawn,
			FText::FromString(TEXT("Build GameSpawn")),
			FText::FromString(TEXT("Build GameSpawn")),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), TEXT("StalkerEditor.BuildGameSpawn"))
		)).SetCommandList(GStalkerEditorManager->UICommandList);
		Section.AddSeparator(NAME_None);
		Section.AddEntry(FToolMenuEntry::InitToolBarButton(StalkerEditorCommands::Get().ReloadConfigsAndScript,
			FText::FromString(TEXT("Reload configs and script")),
			FText::FromString(TEXT("Reload configs and script")),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), TEXT("StalkerEditor.Reload"))
		)).SetCommandList(GStalkerEditorManager->UICommandList);
	}
	else
	{
		UE_LOG(LogStalkerEditor, Error, TEXT("Can't find 'LevelEditor.LevelEditorToolBar.User' menu section"))
	}
}

void FStalkerToolbarMenu::Destroy()
{

}

