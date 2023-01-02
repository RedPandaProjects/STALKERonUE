#pragma once
class StalkerEditorCommands : public TCommands<StalkerEditorCommands>
{
public:

	StalkerEditorCommands();
	void RegisterCommands() override;
public:
	TSharedPtr< FUICommandInfo > BuildCForm;
};