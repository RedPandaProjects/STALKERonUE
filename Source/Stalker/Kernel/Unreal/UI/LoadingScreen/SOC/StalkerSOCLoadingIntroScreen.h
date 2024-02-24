#pragma once

class SStalkerSOCLoadingIntroScreen: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStalkerSOCLoadingIntroScreen) {}

	SLATE_END_ARGS()

	void		Construct				(const FArguments& InArgs);
	void		Tick					(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
private:
	EVisibility GetThrobberVisibility	() const;
	EVisibility GetTextVisibility		() const;
	FSlateColor GetTextColor			() const;
	
	bool		IsLoadEnd = false;
	bool		PressedAnyKey = false;
	bool		CanSkip = false;
	float		CanSkipTimer = 0;
	FColor		TextColor;
};
