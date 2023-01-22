#include "StalkerWayObjectEdModeToolkit.h"
namespace
{
	static const FName WayObjectName(TEXT("WayObject"));
	const TArray<FName> WayObjectPaletteNames = { WayObjectName };
}

FStalkerWayObjectEdModeToolkit::FStalkerWayObjectEdModeToolkit()
{
	SAssignNew(WayObjectEdModeWidget, SStalkerWayObjectEdModeWidget);
}

void FStalkerWayObjectEdModeToolkit::GetToolPaletteNames(TArray<FName>& InPaletteName) const
{
	InPaletteName = WayObjectPaletteNames;
}

FText FStalkerWayObjectEdModeToolkit::GetToolPaletteDisplayName(FName PaletteName) const
{
	if (PaletteName == WayObjectName)
	{
		return FText::FromString("WayObject");
	}
	return FText();
}

void FStalkerWayObjectEdModeToolkit::BuildToolPalette(FName PaletteName, class FToolBarBuilder& ToolbarBuilder)
{
	if (PaletteName == WayObjectName)
	{
		WayObjectEdModeWidget->CustomizeToolBarPalette(ToolbarBuilder);
	}
}

void FStalkerWayObjectEdModeToolkit::SetPropertyEditor(TArray<UObject*>Objects)
{
	WayObjectEdModeWidget->SetPropertyEditor(Objects);
}
