#pragma once
class FStalkerMainMenu
{
public:
	void Initialize();
	void Destroy();
private:
	void MakePulldownMenu(FMenuBarBuilder& menuBuilder);
	void FillPulldownMenu(FMenuBuilder& menuBuilder);
	TSharedPtr<FExtender> MenuExtender;
};