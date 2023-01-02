#include "StalkerEditorCommands.h"
#include "../StalkerEditorStyle.h"

#define LOCTEXT_NAMESPACE "StalkerEditorModule"
StalkerEditorCommands::StalkerEditorCommands() : TCommands<StalkerEditorCommands>(TEXT("StalkerEditor"), NSLOCTEXT("Contexts", "StalkerEditor", "Stalker Editor"), NAME_None, FStalkerEditorStyle::GetStyleSetName())
{

}

void StalkerEditorCommands::RegisterCommands()
{
	UI_COMMAND(BuildCForm, "BuildCForm", "Build CForm", EUserInterfaceActionType::Button, FInputGesture());
}

#undef  LOCTEXT_NAMESPACE 