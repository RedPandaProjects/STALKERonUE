#include "StalkerWayObjectEdModeWidget.h"
#include "StalkerWayObjectEditMode.h"
#include "../../StalkerEditorStyle.h"

#define LOCTEXT_NAMESPACE "StalkerWayObjectEd_Mode"

void SStalkerWayObjectEdModeWidget::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bAllowSearch = false;

	DetailsPanel = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

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
				SNew(SCheckBox)
				.IsEnabled_Lambda([this](){ return GetEdMode()->IsEnabled(); })
				.IsChecked_Lambda([this]() { return GetEdMode()->bAutoLink?   ECheckBoxState::Checked:  ECheckBoxState::Unchecked;  })
				.OnCheckStateChanged_Lambda([this](ECheckBoxState InState) {GetEdMode()->bAutoLink = InState== ECheckBoxState::Checked; })
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
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(2)
				.Padding(2, 0, 0, 0)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Create 1-Link")))
					.IsEnabled_Lambda([this]() {return GetEdMode()->IsEnabledAction(); })
					.OnClicked_Lambda([this]() { GetEdMode()->Add1LinkSelect();  return  FReply::Handled(); })
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
					.Text(FText::FromString(TEXT("Convert to 1-Link")))
					.IsEnabled_Lambda([this]() {return GetEdMode()->IsEnabledAction(); })
					.OnClicked_Lambda([this]() {GetEdMode()->Convert1LinkSelect();  return  FReply::Handled(); })
				/*	.OnClicked(this, &SExampleEdModeWidget::OnAddPoint)
					.IsEnabled(this, &SExampleEdModeWidget::CanAddPoint)*/
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
					.Text(FText::FromString(TEXT("Create 2-Link")))
					.IsEnabled_Lambda([this]() {return GetEdMode()->IsEnabledAction(); })
					.OnClicked_Lambda([this]() { GetEdMode()->Add2LinkSelect();  return  FReply::Handled(); })
				/*	.OnClicked(this, &SExampleEdModeWidget::OnAddPoint)
					.IsEnabled(this, &SExampleEdModeWidget::CanAddPoint)*/
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
					.Text(FText::FromString(TEXT("Convert to 2-Link")))
					.IsEnabled_Lambda([this]() {return GetEdMode()->IsEnabledAction(); })
					.OnClicked_Lambda([this]() {  GetEdMode()->Convert2LinkSelect();  return  FReply::Handled(); })
				/*	.OnClicked(this, &SExampleEdModeWidget::OnAddPoint)
					.IsEnabled(this, &SExampleEdModeWidget::CanAddPoint)*/
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
					.Text(FText::FromString(TEXT("Invert Link")))
					.IsEnabled_Lambda([this]() {return GetEdMode()->IsEnabledAction(); })
					.OnClicked_Lambda([this]() {  GetEdMode()->InvertLinkSelect();  return  FReply::Handled(); })
				/*	.OnClicked(this, &SExampleEdModeWidget::OnAddPoint)
					.IsEnabled(this, &SExampleEdModeWidget::CanAddPoint)*/
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
					.Text(FText::FromString(TEXT("Remove Link")))
					.IsEnabled_Lambda([this]() {return GetEdMode()->IsEnabledAction(); })
					.OnClicked_Lambda([this]() {GetEdMode()->RemoveLinkSelect();   return  FReply::Handled(); })
				/*	.OnClicked(this, &SExampleEdModeWidget::OnAddPoint)
					.IsEnabled(this, &SExampleEdModeWidget::CanAddPoint)*/
				]
			
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 5.f, 0.f, 0.f)
			[
				SNew(SHeader)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Property")))
					.Font(StandardFont)
				]
			]
			+ SVerticalBox::Slot()
			.Padding(0)
			[
				SNew(SVerticalBox)
				.IsEnabled_Lambda([this]() {return GetEdMode()->IsEnabledAction(); })
				+ SVerticalBox::Slot()
				.Padding(0)
				[
					DetailsPanel.ToSharedRef()
				]
			]
		]
		
	];
}

FStalkerWayObjectEditMode* SStalkerWayObjectEdModeWidget::GetEdMode() const
{
	return (FStalkerWayObjectEditMode*)GLevelEditorModeTools().GetActiveMode(FStalkerWayObjectEditMode::EM_WayObject);
}

void SStalkerWayObjectEdModeWidget::CustomizeToolBarPalette(FToolBarBuilder& ToolBarBuilder)
{
	ToolBarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateLambda([this] {	GetEdMode()->SetSelectMode(); }),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([this] {return !GetEdMode()->IsAddPointMode(); })),
		NAME_None,
		FText::FromString(TEXT("Select")),
		FText::FromString(TEXT("Select way object")),
		FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), "StalkerEditor.Select"),
		EUserInterfaceActionType::ToggleButton
	);
	ToolBarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateLambda([this] {	GetEdMode()->SetAddPointMode(); }),
			FCanExecuteAction::CreateLambda([this]{return GetEdMode()->IsEnabled(); }),
			FIsActionChecked::CreateLambda([this] {return GetEdMode()->IsAddPointMode(); })),
		NAME_None,
		FText::FromString(TEXT("Add point")),
		FText::FromString(TEXT("Way point mode")),
		FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), "StalkerEditor.Paint"),
		EUserInterfaceActionType::ToggleButton
	);
}

void SStalkerWayObjectEdModeWidget::SetPropertyEditor(TArray<UObject*> Objs)
{
	DetailsPanel->SetObjects(Objs);
}
