#include "StalkerWayObjectEditorCommands.h"

#define LOCTEXT_NAMESPACE "FAIStalkerMapEditorCommands"
FStalkerWayObjectEditorCommands::FStalkerWayObjectEditorCommands() : TCommands <FStalkerWayObjectEditorCommands>("WayObjectEditor", FText::FromString(TEXT("WayObject Editor")), NAME_None, FEditorStyle::GetStyleSetName())
{
}

FStalkerWayObjectEditorCommands::~FStalkerWayObjectEditorCommands()
{
}

void FStalkerWayObjectEditorCommands::RegisterCommands()
{
	UI_COMMAND(DeleteWay, "Delete way point", "Delete the currently selected way point.", EUserInterfaceActionType::Button, FInputGesture(EKeys::Delete));
	UI_COMMAND(SelectALLWayPoint, "Select all way point", "Select the all way point.", EUserInterfaceActionType::Button, FInputGesture(EKeys::A,false,true,false,false));
}
#undef LOCTEXT_NAMESPACE 
 