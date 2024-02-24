#include "StalkerStartupScreen.h"

#include "MoviePlayer.h"
#include "Widgets/Images/SThrobber.h"
#include "Widgets/Layout/SConstraintCanvas.h"

EVisibility SStalkerStartupScreen::ThrobberVsisble() const
{
	if(GetMoviePlayer()->GetMovieName().Len() != 0)
	{
		return EVisibility::Collapsed;
	}
	return EVisibility::Visible;
}

void SStalkerStartupScreen::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
	   .VAlign(VAlign_Bottom)
	   .Padding(16.0f, 16.0f, 16.0f, 16.0f)
	   [
		   SNew(SHorizontalBox)
		   +SHorizontalBox::Slot()
		   .FillWidth(60)
		   .VAlign(VAlign_Center)
		   .HAlign(HAlign_Right)
		   [
			   SNew(SThrobber)
			   .Visibility_Raw(this,&SStalkerStartupScreen::ThrobberVsisble)
		   ]
		]
	];
}
