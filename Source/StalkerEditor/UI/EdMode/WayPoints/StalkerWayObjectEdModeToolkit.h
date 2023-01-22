#pragma once
#include "StalkerWayObjectEditMode.h"
#include "StalkerWayObjectEdModeWidget.h"
class FStalkerWayObjectEdModeToolkit : public FModeToolkit
{
public:

	FStalkerWayObjectEdModeToolkit();

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override { return FName("WayObjectEdMode"); }
	virtual FText GetBaseToolkitName() const override { return NSLOCTEXT("BuilderModeToolkit", "DisplayName", "Builder"); }
	virtual class FEdMode* GetEditorMode() const override { return GLevelEditorModeTools().GetActiveMode(FStalkerWayObjectEditMode::EM_WayObject); }
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return WayObjectEdModeWidget; }


	void GetToolPaletteNames(TArray<FName>& PaletteNames) const override;


	FText GetToolPaletteDisplayName(FName Palette) const override;


	void BuildToolPalette(FName Palette, class FToolBarBuilder& ToolbarBuilder) override;

	void SetPropertyEditor(TArray<UObject*>Objects);

private:

	TSharedPtr<SStalkerWayObjectEdModeWidget> WayObjectEdModeWidget;

};
