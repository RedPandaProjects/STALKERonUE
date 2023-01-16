#pragma once
class FStalkerAIMapEditorCommands : public TCommands<FStalkerAIMapEditorCommands>
{
public:
	FStalkerAIMapEditorCommands();
	~FStalkerAIMapEditorCommands();
	void RegisterCommands() override;
	TSharedPtr<FUICommandInfo> DeleteNode;
};
