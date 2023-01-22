#pragma once
#include "Widgets/SCompoundWidget.h"


class SStalkerWayObjectEdModeWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStalkerWayObjectEdModeWidget) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs);
	
	// Util Functions
	class FStalkerWayObjectEditMode* GetEdMode() const;

	void CustomizeToolBarPalette(FToolBarBuilder& ToolBarBuilder);
	void SetPropertyEditor(TArray<UObject*>Objects);

	TSharedPtr<IDetailsView> DetailsPanel;
};