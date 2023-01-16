#pragma once
#include "Widgets/SCompoundWidget.h"


class SStalkerAIMapEdModeWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStalkerAIMapEdModeWidget) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs);
	
	// Util Functions
	class FStalkerAIMapEditMode* GetEdMode() const;

	void CustomizeToolBarPalette(FToolBarBuilder& ToolBarBuilder);
	void OnSelectMode();
	void OnAddMode();
};