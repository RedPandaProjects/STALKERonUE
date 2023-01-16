#include "StalkerAIMapEdModeToolkit.h"
namespace
{
	static const FName AIMapName(TEXT("AIMap"));
	const TArray<FName> AIMapPaletteNames = { AIMapName };
}

FStalkerAIMapEdModeToolkit::FStalkerAIMapEdModeToolkit()
{
	SAssignNew(AIMapEdModeWidget, SStalkerAIMapEdModeWidget);
}

void FStalkerAIMapEdModeToolkit::GetToolPaletteNames(TArray<FName>& InPaletteName) const
{
	InPaletteName = AIMapPaletteNames;
}

FText FStalkerAIMapEdModeToolkit::GetToolPaletteDisplayName(FName PaletteName) const
{
	if (PaletteName == AIMapName)
	{
		return FText::FromString("AIMap");
	}
	return FText();
}

void FStalkerAIMapEdModeToolkit::BuildToolPalette(FName PaletteName, class FToolBarBuilder& ToolbarBuilder)
{
	if (PaletteName == AIMapName)
	{
		AIMapEdModeWidget->CustomizeToolBarPalette(ToolbarBuilder);
	}
}
