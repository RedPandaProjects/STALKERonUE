#include "StalkerToolbarMenu.h"
#include "../StalkerEditorStyle.h"
#include "../Commands/StalkerEditorCommands.h"
void StalkerToolbarMenu::Initialize()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	if (UToolMenu* ProfileMenu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.LevelEditorToolBar.User")))
	{
		FToolMenuSection& Section = ProfileMenu->AddSection("Stalker Tools", FText::FromString(TEXT("Stalker Tools")));
		//Section.AddSeparator(NAME_None);
		/*	Section.AddEntry(FToolMenuEntry::InitToolBarButton(StalkerEditorCommands::Get().BuildCForm,
				FText::FromString(TEXT("Build CForm")),
				FText::FromString(TEXT("Build CForm")),
				FSlateIcon(FStalkerEditorStyle::GetStyleSetName(),TEXT("StalkerEditor.BuildCForm"))
				));*/
	}
	else
	{
		UE_LOG(LogStalkerEditor, Error, TEXT("Can't find 'LevelEditor.LevelEditorToolBar.User' menu section"))
	}
}

void StalkerToolbarMenu::Destroy()
{

}

