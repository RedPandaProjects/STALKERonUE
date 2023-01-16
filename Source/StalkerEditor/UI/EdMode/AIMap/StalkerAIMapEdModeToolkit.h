#pragma once
#include "StalkerAIMapEdModeWidget.h"
#include "StalkerAIMapEditMode.h"
class FStalkerAIMapEdModeToolkit : public FModeToolkit
{
public:

	FStalkerAIMapEdModeToolkit();

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override { return FName("AIMapEdMode"); }
	virtual FText GetBaseToolkitName() const override { return NSLOCTEXT("BuilderModeToolkit", "DisplayName", "Builder"); }
	virtual class FEdMode* GetEditorMode() const override { return GLevelEditorModeTools().GetActiveMode(FStalkerAIMapEditMode::EM_AIMap); }
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return AIMapEdModeWidget; }


	void GetToolPaletteNames(TArray<FName>& PaletteNames) const override;


	FText GetToolPaletteDisplayName(FName Palette) const override;


	void BuildToolPalette(FName Palette, class FToolBarBuilder& ToolbarBuilder) override;

private:

	TSharedPtr<SStalkerAIMapEdModeWidget> AIMapEdModeWidget;

};
