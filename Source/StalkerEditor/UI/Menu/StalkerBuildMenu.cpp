#include "StalkerBuildMenu.h"
#include "../Commands/StalkerEditorCommands.h"
#include "../StalkerEditorStyle.h"

void StalkerBuildMenu::Initialize()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	if (UToolMenu* ProfileMenu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Build")))
	{
		FToolMenuSection& Section = ProfileMenu->AddSection("Stalker Build", FText::FromString(TEXT("Stalker Build")));

		/*Section.AddMenuEntry(StalkerEditorCommands::Get().BuildCForm,
			FText::FromString(TEXT("Build CForm")),
			FText::FromString(TEXT("Build CForm")),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), TEXT("StalkerEditor.BuildCForm"))
		);*/
	}
	else
	{
		UE_LOG(LogStalkerEditor, Error, TEXT("Can't find 'MainFrame.MainMenu.Build' menu section"))
	}
}

void StalkerBuildMenu::Destroy()
{

}

