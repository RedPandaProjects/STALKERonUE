#include "StalkerAIMapEdModeWidget.h"
#include "StalkerAIMapEditMode.h"
#include "../../StalkerEditorStyle.h"

#define LOCTEXT_NAMESPACE "StalkerAIMapEd_Mode"

void SStalkerAIMapEdModeWidget::Construct(const FArguments& InArgs)
{
	FSlateFontInfo StandardFont = FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont"));
	FSlateFontInfo SmallFont = FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont"));
	SmallFont.Size = 8;
	ChildSlot
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		.VAlign(VAlign_Top)
		.Padding(5.f)
		[

			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 5.f, 0.f, 0.f)
			[
				SNew(SHeader)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("General")))
					.Font(StandardFont)
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 5.f, 0.f, 0.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(2)
				.Padding(2, 0, 0, 0)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Generate Full")))
					.OnClicked_Lambda([this]() { GetEdMode()->GenerateFull(); return  FReply::Handled(); })
					.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
				]
			
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 5.f, 0.f, 0.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(2)
				.Padding(2, 0, 0, 0)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Generate Select")))
					.OnClicked_Lambda([this]() { GetEdMode()->GenerateSelect(); return  FReply::Handled(); })
					.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
				]
			
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 5.f, 0.f, 0.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(2)
				.Padding(2, 0, 0, 0)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Smooth Full")))
					.OnClicked_Lambda([this]() { GetEdMode()->SmoothFull(); return  FReply::Handled(); })
					.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
				]
			
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 5.f, 0.f, 0.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(2)
				.Padding(2, 0, 0, 0)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Smooth Select")))
					.OnClicked_Lambda([this]() { GetEdMode()->SmoothSelect(); return  FReply::Handled(); })
					.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
				]
			
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 5.f, 0.f, 0.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(2)
				.Padding(2, 0, 0, 0)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Reset Select")))
					.OnClicked_Lambda([this]() { GetEdMode()->ResetSelect(); return  FReply::Handled(); })
					.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
				]
			
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 5.f, 0.f, 0.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(2)
				.Padding(2, 0, 0, 0)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Clear AI Map")))
					.OnClicked_Lambda([this](){ GetEdMode()->ClearAIMap();return  FReply::Handled();})
					.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
				]
			
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 5.f, 0.f, 0.f)
			[
				SNew(SHeader)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Settings")))
					.Font(StandardFont)
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 5.f, 0.f, 0.f)
			[
				SNew(SCheckBox)
				.IsChecked_Lambda([this]() { return GetEdMode()->bIgnoreConstraints? ECheckBoxState::Checked:ECheckBoxState::Unchecked; })
				.OnCheckStateChanged_Lambda([this](ECheckBoxState InState) {GetEdMode()->bIgnoreConstraints = InState==ECheckBoxState::Checked;})
				.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabled(); })
				[
					SNew(STextBlock)
					.Font(StandardFont)
					.Text(FText::FromString(TEXT("Ignore Constraints")))
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 5.f, 0.f, 0.f)
			[
				SNew(SCheckBox)
				.IsChecked_Lambda([this]() { return GetEdMode()->bAutoLink ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
				.OnCheckStateChanged_Lambda([this](ECheckBoxState InState) {GetEdMode()->bAutoLink = InState == ECheckBoxState::Checked; })
				.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabled(); })
				[
					SNew(STextBlock)
					.Font(StandardFont)
					.Text(FText::FromString(TEXT("Auto Link")))
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 5.f, 0.f, 0.f)
			[
				SNew(SHeader)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Link Tools")))
					.Font(StandardFont)
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 5.f, 0.f, 0.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.f, 5.f, 0.f, 0.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SCheckBox).Style(FAppStyle::Get(), "RadioButton")
							.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
							.IsChecked_Lambda([this]() { return GetEdMode()->LinkMode == FStalkerAIMapEditMode::LM_Add? ECheckBoxState::Checked: ECheckBoxState::Unchecked; })
							.OnCheckStateChanged_Lambda([this](ECheckBoxState InState) {if (InState == ECheckBoxState::Checked) { GetEdMode()->LinkMode = FStalkerAIMapEditMode::LM_Add; } })
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("Add")))
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.f, 5.f, 0.f, 0.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SCheckBox).Style(FAppStyle::Get(), "RadioButton")
							.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
							.IsChecked_Lambda([this]() { return  GetEdMode()->LinkMode == FStalkerAIMapEditMode::LM_Remove ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;  })
							.OnCheckStateChanged_Lambda([this](ECheckBoxState InState) {if (InState == ECheckBoxState::Checked) { GetEdMode()->LinkMode = FStalkerAIMapEditMode::LM_Remove; }})
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("Remove")))
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.f, 5.f, 0.f, 0.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SCheckBox).Style(FAppStyle::Get(), "RadioButton")
							.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
							.IsChecked_Lambda([this]() { return GetEdMode()->LinkMode == FStalkerAIMapEditMode::LM_Invert ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; ; })
							.OnCheckStateChanged_Lambda([this](ECheckBoxState InState) { if(InState== ECheckBoxState::Checked) { GetEdMode()->LinkMode = FStalkerAIMapEditMode::LM_Invert; }})
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("Invert")))
						]

			
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.f, 5.f, 0.f, 0.f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot().MaxHeight(20)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.MaxWidth(20)
						.VAlign(VAlign_Center)
						[
							SNew(SButton)
							.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
							.Text(FText::FromString(TEXT("X"))).Visibility(EVisibility::Hidden)
						]
						+ SHorizontalBox::Slot()
						.MaxWidth(20)
						.VAlign(VAlign_Center)
						[
							SNew(SButton)
							.ContentPadding(FMargin(0.0f, 0.0f))
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.VAlign(VAlign_Center).HAlign(HAlign_Center)
								.Padding(-2.0f, 2.0f, 2.0f, 2.0f)
								.AutoWidth()
								[
									SNew(STextBlock)
									.Text(FText::FromString(TEXT("\u2191")))//
									.Font(SmallFont)
								]
							]
							.OnClicked_Lambda([this]() { GetEdMode()->LinkForward(); return  FReply::Handled(); })
							.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
						]
					]
					+ SVerticalBox::Slot().MaxHeight(20)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.MaxWidth(20)
						.VAlign(VAlign_Center)
						[
							SNew(SButton)
							.ContentPadding(FMargin(0.0f, 0.0f))
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.VAlign(VAlign_Center).HAlign(HAlign_Center)
								.Padding(-4.0f, 2.0f, 2.0f, 2.0f)
								.AutoWidth()
								[
									SNew(STextBlock)
									.Text(FText::FromString(TEXT("\u2190")))//
									.Font(SmallFont)
								]
							]
							.OnClicked_Lambda([this]() { GetEdMode()->LinkLeft(); return  FReply::Handled(); })
							.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
						]
						+ SHorizontalBox::Slot()
						.MaxWidth(20)
						.VAlign(VAlign_Center)
						[
							SNew(SButton)
							.ContentPadding(FMargin(0.0f, 0.0f))
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.VAlign(VAlign_Center).HAlign(HAlign_Center)
								.Padding(-3.0f, 3.0f, 2.0f, 2.0f)
								.AutoWidth()
								[
									SNew(STextBlock)
									.Text(FText::FromString(TEXT("X")))//
									.Font(SmallFont)
								]
							]
							.OnClicked_Lambda([this]() { GetEdMode()->LinkAll(); return  FReply::Handled(); })
							.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
						]
						+ SHorizontalBox::Slot()
						.MaxWidth(20)
						.VAlign(VAlign_Center)
						[
							SNew(SButton)
							.ContentPadding(FMargin(0.0f, 0.0f))
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.VAlign(VAlign_Center).HAlign(HAlign_Center)
								.Padding(-4.0f, 2.0f, 2.0f, 2.0f)
								.AutoWidth()
								[
									SNew(STextBlock)
									.Text(FText::FromString(TEXT("\u2192")))//
									.Font(SmallFont)
								]
							]
							.OnClicked_Lambda([this]() { GetEdMode()->LinkRight(); return  FReply::Handled(); })
							.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
						]
					]
					+ SVerticalBox::Slot().MaxHeight(20)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.MaxWidth(20)
						.VAlign(VAlign_Center)
						[
							SNew(SButton)
							.Text(FText::FromString(TEXT("X"))).Visibility(EVisibility::Hidden)
						]
						+SHorizontalBox::Slot()
						.MaxWidth(20)
						.VAlign(VAlign_Center)
						[
							SNew(SButton)
							.ContentPadding(FMargin(0.0f, 0.0f))
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.VAlign(VAlign_Center).HAlign(HAlign_Center)
								.Padding(-2.0f, 2.0f, 2.0f, 2.0f)
								.AutoWidth()
								[
									SNew(STextBlock)
									.Text(FText::FromString(TEXT("\u2193")))//
									.Font(SmallFont)
								]
							]
							.OnClicked_Lambda([this]() { GetEdMode()->LinkBackward(); return  FReply::Handled(); })
							.IsEnabled_Lambda([this]() { return GetEdMode()->IsEnabledAction(); })
						]
					]
				]
			]
		]
	];
}

FStalkerAIMapEditMode* SStalkerAIMapEdModeWidget::GetEdMode() const
{
	return (FStalkerAIMapEditMode*)GLevelEditorModeTools().GetActiveMode(FStalkerAIMapEditMode::EM_AIMap);
}

void SStalkerAIMapEdModeWidget::CustomizeToolBarPalette(FToolBarBuilder& ToolBarBuilder)
{
	ToolBarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateLambda([this] {	GetEdMode()->SetSelectionMode();}),
		FCanExecuteAction(),
		FIsActionChecked::CreateLambda([this] {return !GetEdMode()->IsAddMode();})),
		NAME_None,
		FText::FromString(TEXT("Select")),
		FText::FromString(TEXT("Select AIMap")),
		FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), "StalkerEditor.Select"),
		EUserInterfaceActionType::ToggleButton
	);
	ToolBarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateLambda([this] {	GetEdMode()->SetAddMode(); }),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([this] {return GetEdMode()->IsAddMode(); })),
		NAME_None,
		FText::FromString(TEXT("Add")),
		FText::FromString(TEXT("Add Node AIMap")),
		FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), "StalkerEditor.Paint"),
		EUserInterfaceActionType::ToggleButton
	);
}

void SStalkerAIMapEdModeWidget::OnSelectMode()
{

}

void SStalkerAIMapEdModeWidget::OnAddMode()
{

}
