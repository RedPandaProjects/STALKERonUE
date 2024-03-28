#include "StalkerMainMenu.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"
#include "Framework/Commands/Commands.h"
#include "../StalkerEditorStyle.h"
#include "../../StalkerEditorManager.h"
#include "../Commands/StalkerEditorCommands.h"

void FStalkerMainMenu::Initialize()
{
	if (!IsRunningCommandlet())
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		auto LevelEditorMenuExtensibilityManager = LevelEditorModule.GetMenuExtensibilityManager();
		MenuExtender = MakeShareable(new FExtender);
		MenuExtender->AddMenuBarExtension("Help", EExtensionHook::After, GStalkerEditorManager->UICommandList, FMenuBarExtensionDelegate::CreateRaw(this, &FStalkerMainMenu::MakePulldownMenu));
		LevelEditorMenuExtensibilityManager->AddExtender(MenuExtender);
	}
}

void FStalkerMainMenu::Destroy()
{
}

void FStalkerMainMenu::MakePulldownMenu(FMenuBarBuilder& InMenuBuilder)
{
	InMenuBuilder.AddPullDownMenu(
		FText::FromString("Stalker"),
		FText::FromString("Open the Stalker menu"),
		FNewMenuDelegate::CreateRaw(this, &FStalkerMainMenu::FillPulldownMenu),
		"Stalker");
}

void FStalkerMainMenu::FillPulldownMenu(FMenuBuilder& InMenuBuilder)
{
	InMenuBuilder.BeginSection(NAME_None, FText::FromString("Games"));
	{
		// Function for checking if Release and Debug versions of a file
		auto IsFileExists = [](const FString& BaseFileName) -> bool
		{
			FString ReleaseFilePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Binaries"), TEXT("Win64"), BaseFileName + TEXT("_Release.dll"));
			FString DebugFilePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Binaries"), TEXT("Win64"), BaseFileName + TEXT("_Debug.dll"));

			return FPaths::FileExists(ReleaseFilePath) || FPaths::FileExists(DebugFilePath);
		};

		// Function to create a tooltip text attribute based on the presence of a file
		auto CreateTooltipTextAttribute = [&](const FString& TempBaseFileName) -> TAttribute<FText>
		{
			return TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateLambda([IsFileExists, BaseFileName = TempBaseFileName]() -> FText
			{
				if (!IsFileExists(BaseFileName))
				{
					return FText::FromString(FString::Printf(TEXT("%s not builded"), *BaseFileName));
				}
				return FText::GetEmpty();
			}));
		};

		// Actions for Call of Pripyat
		{
			FUIAction CoPAction(
				FExecuteAction::CreateLambda([]()
				{
					UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
					if (SGSettings->EditorStartupGame != EStalkerGame::COP)
					{
						SGSettings->EditorStartupGame = EStalkerGame::COP;
						SGSettings->TryUpdateDefaultConfigFile();
						SGSettings->ReInitilizeXRay();
					}
				}),
				FCanExecuteAction::CreateLambda([IsFileExists]()
				{
					return IsFileExists(TEXT("XrGame"));
				}),
				FIsActionChecked::CreateLambda([]()
				{
					const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>();
					return SGSettings->EditorStartupGame == EStalkerGame::COP;
				}));

			InMenuBuilder.AddMenuEntry(
				FText::FromString("Call of Pripyat"),
				CreateTooltipTextAttribute(TEXT("XrGame")),
				FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), "StalkerEditor.COP"),
				CoPAction,
				NAME_None,
				EUserInterfaceActionType::RadioButton);
		}

		// Actions for Clear Sky
		{
			FUIAction CSAction(
				FExecuteAction::CreateLambda([]()
				{
					UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
					if (SGSettings->EditorStartupGame != EStalkerGame::CS)
					{
						SGSettings->EditorStartupGame = EStalkerGame::CS;
						SGSettings->TryUpdateDefaultConfigFile();
						SGSettings->ReInitilizeXRay();
					}
				}),
				FCanExecuteAction::CreateLambda([IsFileExists]()
				{
					return IsFileExists(TEXT("XrGameCS"));
				}),
				FIsActionChecked::CreateLambda([]()
				{
					const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>();
					return SGSettings->EditorStartupGame == EStalkerGame::CS;
				}));

			InMenuBuilder.AddMenuEntry(
				FText::FromString("Clear Sky"),
				CreateTooltipTextAttribute(TEXT("XrGameCS")),
				FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), "StalkerEditor.CS"),
				CSAction,
				NAME_None,
				EUserInterfaceActionType::RadioButton);
		}

		// Actions for Shadow of Chernobyl
		{
			FUIAction SOCAction(
				FExecuteAction::CreateLambda([]()
				{
					UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
					if (SGSettings->EditorStartupGame != EStalkerGame::SHOC)
					{
						SGSettings->EditorStartupGame = EStalkerGame::SHOC;
						SGSettings->TryUpdateDefaultConfigFile();
						SGSettings->ReInitilizeXRay();
					}
				}),
				FCanExecuteAction::CreateLambda([IsFileExists]()
				{
					return IsFileExists(TEXT("XrGameSOC"));
				}),
				FIsActionChecked::CreateLambda([]()
				{
					const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>();
					return SGSettings->EditorStartupGame == EStalkerGame::SHOC;
				}));

			InMenuBuilder.AddMenuEntry(
				FText::FromString("Shadow of Chernobyl"),
				CreateTooltipTextAttribute(TEXT("XrGameSOC")),
				FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), "StalkerEditor.SOC"),
				SOCAction,
				NAME_None,
				EUserInterfaceActionType::RadioButton);
		}
	}
	InMenuBuilder.EndSection();

	InMenuBuilder.BeginSection(NAME_None, FText::FromString("Tools"));
	{
		InMenuBuilder.AddSubMenu(
			FText::FromString("1. Import Textures"),
			FText::FromString("Import different types of textures"),
			FNewMenuDelegate::CreateLambda([](FMenuBuilder& SubMenuBuilder)
			{
				SubMenuBuilder.AddMenuEntry(StalkerEditorCommands::Get().ImportAllTextures, "ImportAllTextures", FText::FromString("Import All Textures"));
				SubMenuBuilder.AddMenuEntry(StalkerEditorCommands::Get().ImportUITextures, "ImportUITextures", FText::FromString("Import UI Textures"));
				SubMenuBuilder.AddMenuEntry(StalkerEditorCommands::Get().ImportWMTextures, "ImportWMTextures", FText::FromString("Import Wallmarks Textures"));
				SubMenuBuilder.AddMenuEntry(StalkerEditorCommands::Get().ImportGradTextures, "ImportGradTextures", FText::FromString("Import Grad Textures"));
			}));
		InMenuBuilder.AddMenuEntry(StalkerEditorCommands::Get().ImportPhysicalMaterials, "ImportPhysicalMaterials", FText::FromString("2. Import Physical Materials"));
		InMenuBuilder.AddMenuEntry(StalkerEditorCommands::Get().ImportMeshes, "ImportMeshes", FText::FromString("3. Import Meshes"));
		InMenuBuilder.AddMenuEntry(StalkerEditorCommands::Get().ImportParticles, "ImportParticles", FText::FromString("4. Import Particles"));
		InMenuBuilder.AddMenuEntry(StalkerEditorCommands::Get().ImportSounds, "ImportSounds", FText::FromString("5. Import Sounds"));
		FUIAction Action(FExecuteAction::CreateLambda(
			[]()
			{
				UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
				SGSettings->AlwaysOverwriteAssetsWhenImport = !SGSettings->AlwaysOverwriteAssetsWhenImport;
				SGSettings->PostEditChange();
				SGSettings->SaveConfig();
			}),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([]()
			{
				UStalkerGameSettings* SGSettings = GetMutableDefault<UStalkerGameSettings>();
				return SGSettings->AlwaysOverwriteAssetsWhenImport;
			}));
		InMenuBuilder.AddMenuEntry(FText::FromString(TEXT("Always overwrite assets")),
			FText::FromString(TEXT("Always overwrite assets when import")), FSlateIcon(), Action,
			NAME_None, EUserInterfaceActionType::Check);
	}

	InMenuBuilder.EndSection();

	InMenuBuilder.BeginSection(NAME_None, FText::FromString("Build"));
	{
		InMenuBuilder.AddMenuEntry(
			StalkerEditorCommands::Get().BuildCForm,
			"BuildCForm",
			FText::FromString("Build CForm"),
			FText::GetEmpty(),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), "StalkerEditor.BuildCForm"));

		InMenuBuilder.AddMenuEntry(
			StalkerEditorCommands::Get().BuildAIMap,
			"BuildAIMap",
			FText::FromString("Build AIMap"),
			FText::GetEmpty(),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), "StalkerEditor.BuildAIMap"));

		InMenuBuilder.AddMenuEntry(
			StalkerEditorCommands::Get().BuildLevelSpawn,
			"BuildLevelSpawn",
			FText::FromString("Build LevelSpawn"),
			FText::GetEmpty(),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), "StalkerEditor.BuildLevelSpawn"));

		InMenuBuilder.AddMenuEntry(
			StalkerEditorCommands::Get().BuildGameSpawn,
			"BuildGameSpawn",
			FText::FromString("Build GameSpawn"),
			FText::GetEmpty(),
			FSlateIcon(FStalkerEditorStyle::GetStyleSetName(), "StalkerEditor.BuildGameSpawn"));
	}

	InMenuBuilder.EndSection();
}
