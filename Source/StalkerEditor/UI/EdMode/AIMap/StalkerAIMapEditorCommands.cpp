#include "StalkerAIMapEditorCommands.h"

#define LOCTEXT_NAMESPACE "FAIStalkerMapEditorCommands"
FStalkerAIMapEditorCommands::FStalkerAIMapEditorCommands() : TCommands <FStalkerAIMapEditorCommands>("AIMapEditor", FText::FromString(TEXT("AIMap Editor")), NAME_None, FEditorStyle::GetStyleSetName())
{
}

FStalkerAIMapEditorCommands::~FStalkerAIMapEditorCommands()
{
}

void FStalkerAIMapEditorCommands::RegisterCommands()
{
	UI_COMMAND(DeleteNode, "Delete Node", "Delete the currently selected node.", EUserInterfaceActionType::Button, FInputGesture(EKeys::Delete));
}
#undef LOCTEXT_NAMESPACE 
 