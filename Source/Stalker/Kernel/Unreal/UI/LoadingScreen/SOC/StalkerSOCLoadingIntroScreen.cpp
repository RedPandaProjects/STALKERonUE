#include "StalkerSOCLoadingIntroScreen.h"

#include "MoviePlayer.h"
#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"
#include "Resources/LoadingScreen/StalkerLoadingScreenManager.h"
#include "Widgets/Images/SThrobber.h"

#define LOCTEXT_NAMESPACE "StalkerModule"
void SStalkerSOCLoadingIntroScreen::Construct(const FArguments& InArgs)
{
	TextColor = FColor::White;
	SetCursor(EMouseCursor::None);
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
	   .VAlign(VAlign_Bottom)
	   .Padding(8.0f, 8.0f, 8.0f, 8.0f)
	   [
		   SNew(SHorizontalBox)
		   +SHorizontalBox::Slot()
		   .FillWidth(1)
		   .HAlign(HAlign_Right)
		   [
				SNew(STextBlock)
        		.Text(LOCTEXT("SStalkerLoadingIntroScreen","Press any key for skip movie"))
        		.Justification(ETextJustify::InvariantRight)
				.Visibility_Raw(this,&SStalkerSOCLoadingIntroScreen::GetTextVisibility)
				.ColorAndOpacity_Raw(this,&SStalkerSOCLoadingIntroScreen::GetTextColor)
        		.Font(FSlateFontInfo(GStalkerEngineManager->GetResourcesManager()->LoadingScreenManager.MainFont,12))
				
			]
		   +SHorizontalBox::Slot()
		   .FillWidth(20)
		   .VAlign(VAlign_Center)
		   .HAlign(HAlign_Right)
		   [
			   SNew(SThrobber)
			   .Visibility_Raw(this,&SStalkerSOCLoadingIntroScreen::GetThrobberVisibility)
		   ]
		]
	];
}

void SStalkerSOCLoadingIntroScreen::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	FStalkerLoadingUpdateData UpdateData;
	while(GStalkerEngineManager->GetResourcesManager()->LoadingScreenManager.UpdateData.Dequeue(UpdateData))
	{
		switch (UpdateData.Type)
		{
		case EStalkerLoadingUpdateDataType::EndLoad:
			IsLoadEnd = true;
			break;
		case EStalkerLoadingUpdateDataType::PressAnyKey:
			PressedAnyKey = true;
			break;
		default:
			break;
		}
	}
	if(IsLoadEnd)
	{
		if(PressedAnyKey&&!CanSkip)
		{
			PressedAnyKey = false;
			CanSkip = true;
			CanSkipTimer = 0;
		}
		else if(PressedAnyKey&&CanSkip)
		{
			GetMoviePlayer()->StopMovie();
		}
		else if(GetMoviePlayer()->GetMovieName().Len() == 0)
		{
			GetMoviePlayer()->StopMovie();
		}
	}
	if(CanSkip)
	{
		CanSkipTimer+=InDeltaTime;
		if(CanSkipTimer>6.f&&TextColor.A<20)
		{
			CanSkip = false;
			TextColor.A = 255;
		}
		else
		{
			TextColor.A = FMath::Abs(FMath::Sin(InCurrentTime*UE_PI))*255;
		}
	}
}


EVisibility SStalkerSOCLoadingIntroScreen::GetTextVisibility() const
{
	return !CanSkip?EVisibility::Collapsed:EVisibility::Visible;
}

FSlateColor SStalkerSOCLoadingIntroScreen::GetTextColor() const
{
	return TextColor;
}
EVisibility SStalkerSOCLoadingIntroScreen::GetThrobberVisibility() const
{
	return IsLoadEnd?EVisibility::Collapsed:EVisibility::Visible;
}
#undef LOCTEXT_NAMESPACE