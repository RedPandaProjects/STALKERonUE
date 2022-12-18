#include "StalkerMainMenu.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"

void StalkerMainMenu::Initialize()
{
	if (!IsRunningCommandlet())
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		auto LevelEditorMenuExtensibilityManager = LevelEditorModule.GetMenuExtensibilityManager();
		MenuExtender = MakeShareable(new FExtender);
		MenuExtender->AddMenuBarExtension("Window", EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &StalkerMainMenu::MakePulldownMenu));
		LevelEditorMenuExtensibilityManager->AddExtender(MenuExtender);


		MenuExtender->AddMenuExtension(FName("GameSection"), EExtensionHook::Before, nullptr, FMenuExtensionDelegate::CreateRaw(this, &StalkerMainMenu::MakeMenuGame));
	}
}

void StalkerMainMenu::Destroy()
{

}

void StalkerMainMenu::MakePulldownMenu(FMenuBarBuilder& InMenuBuilder)
{
	InMenuBuilder.AddPullDownMenu(
		FText::FromString("Stalker"),
		FText::FromString("Open the Stalker menu"),
		FNewMenuDelegate::CreateRaw(this, &StalkerMainMenu::FillPulldownMenu),
		"Stalker",
		FName(TEXT("StalkerMenu"))
	);
}

void StalkerMainMenu::FillPulldownMenu(FMenuBuilder& InMenuBuilder)
{
	InMenuBuilder.BeginSection("Games", FText::FromString("Games"));
	InMenuBuilder.AddMenuSeparator(FName("GameSection"));
	InMenuBuilder.EndSection();
}

void StalkerMainMenu::MakeMenuGame(FMenuBuilder& InMenuBuilder)
{
	TSharedRef<SWidget> AddTagWidget =
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
				.IsEnabled(true)
				.IsChecked_Lambda([this]() {const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>(); return 	SGSettings->EditorStartupGame == EStalkerGame::COP? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
				.OnCheckStateChanged_Lambda([this](ECheckBoxState InState) {if (InState == ECheckBoxState::Checked) { UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();SGSettings->EditorStartupGame = EStalkerGame::COP;SGSettings->TryUpdateDefaultConfigFile();SGSettings->ReInitilizeXRay(); } })
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Call of Pripyat")))
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
				.IsEnabled(true) 
				.IsChecked_Lambda([this]() {const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>(); return 	SGSettings->EditorStartupGame == EStalkerGame::CS ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
				.OnCheckStateChanged_Lambda([this](ECheckBoxState InState) {if (InState == ECheckBoxState::Checked) { UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>(); SGSettings->EditorStartupGame = EStalkerGame::CS; SGSettings->TryUpdateDefaultConfigFile(); SGSettings->ReInitilizeXRay();} })
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Clear Sky")))
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
				.IsEnabled(true) 
				.IsChecked_Lambda([this]() {const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>(); return 	SGSettings->EditorStartupGame == EStalkerGame::SHOC ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
				.OnCheckStateChanged_Lambda([this](ECheckBoxState InState) {if (InState == ECheckBoxState::Checked) { UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>(); SGSettings->EditorStartupGame = EStalkerGame::SHOC; SGSettings->TryUpdateDefaultConfigFile(); SGSettings->ReInitilizeXRay();
		} })

			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Shadow of Chernobyl")))
			]
		];

	

	InMenuBuilder.AddWidget(AddTagWidget, FText::FromString(""));
}
