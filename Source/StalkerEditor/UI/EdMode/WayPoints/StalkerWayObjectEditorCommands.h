#pragma once
class FStalkerWayObjectEditorCommands : public TCommands<FStalkerWayObjectEditorCommands>
{
public:
	FStalkerWayObjectEditorCommands();
	~FStalkerWayObjectEditorCommands();
	void RegisterCommands() override;
	TSharedPtr<FUICommandInfo> DeleteWay;
	TSharedPtr<FUICommandInfo> SelectALLWayPoint;
};
