#pragma once
#include "Slate/DeferredCleanupSlateBrush.h"

class SStalkerSOCLoadingScreen: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStalkerSOCLoadingScreen) {}
	SLATE_END_ARGS()

	void							Construct				(const FArguments& InArgs,int32 MaxStatus);
	int32							OnPaint					(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,	bool bParentEnabled) const override;
	void							Tick					(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
private:					
	FText							GetLoadingStatusText	() const;
	EVisibility 					GetThrobberVisibility	() const;
	FSlateColor 					GetLoadingStatusColor	() const;
	
	TSharedPtr< ISlateBrushSource > BackgroundSlateBrush;
	TSharedPtr< ISlateBrushSource > MapSlateBrush;
	FText 							StatusText;
	int32 							MaxStatus = 1;
	int32 							IndexStatus = 0;
	bool							IsLoadEnd = false;
	FColor							StatusTextColor;
};
