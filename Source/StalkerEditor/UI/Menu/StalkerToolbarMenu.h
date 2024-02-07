#pragma once
class FStalkerToolbarMenu
{
public:
	void								Initialize	();
	void								Destroy		();
private:
	TSharedPtr<const FExtensionBase>	ToolbarExtension;
	TSharedPtr<FExtensibilityManager>	ExtensionManager;
	TSharedPtr<FExtender>				ToolbarExtender;
};