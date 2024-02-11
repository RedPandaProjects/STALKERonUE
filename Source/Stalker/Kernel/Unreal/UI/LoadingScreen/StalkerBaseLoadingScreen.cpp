#include "StalkerBaseLoadingScreen.h"
#include "Widgets/Images/SThrobber.h"
#include "Widgets/Layout/SConstraintCanvas.h"

void SStalkerBaseLoadingScreen::Construct(const FArguments& InArgs)
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
		   ]
		]
	];
}
