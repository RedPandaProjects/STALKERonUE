#include "StalkerToolsMenu.h"
#include "../Commands/StalkerEditorCommands.h"
#include "../StalkerEditorStyle.h"
#include "../../StalkerEditorManager.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"

void FStalkerToolsMenu::Initialize()
{
    FToolMenuOwnerScoped OwnerScoped(this);

    if (UToolMenu* ProfileMenu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Tools")))
    {
        FToolMenuSection& Section = ProfileMenu->AddSection("Stalker Tools", FText::FromString(TEXT("Stalker Tools")));

        Section.AddSubMenu(
            "ImportTextures",
            FText::FromString(TEXT("1. Import Textures")),
            FText::FromString(TEXT("Import various types of textures")),
            FNewToolMenuDelegate::CreateLambda([](UToolMenu* InMenu)
            {
                FToolMenuSection& SubMenuSection = InMenu->AddSection("ImportTexturesSection");
                SubMenuSection.AddMenuEntry(StalkerEditorCommands::Get().ImportAllTextures,
                    FText::FromString(TEXT("Import All Textures")),
                    FText::FromString(TEXT("Import All Textures")),
                    FSlateIcon()
                ).SetCommandList(GStalkerEditorManager->UICommandList);

                SubMenuSection.AddMenuEntry(StalkerEditorCommands::Get().ImportUITextures,
                    FText::FromString(TEXT("Import UI Textures")),
                    FText::FromString(TEXT("Import UI Textures")),
                    FSlateIcon()
                ).SetCommandList(GStalkerEditorManager->UICommandList);

                SubMenuSection.AddMenuEntry(StalkerEditorCommands::Get().ImportWMTextures,
                    FText::FromString(TEXT("Import Wallmark Textures")),
                    FText::FromString(TEXT("Import Wallmark Textures")),
                    FSlateIcon()
                ).SetCommandList(GStalkerEditorManager->UICommandList);
				
				SubMenuSection.AddMenuEntry(StalkerEditorCommands::Get().ImportGradTextures,
					FText::FromString(TEXT("Import Grad Textures")),
					FText::FromString(TEXT("Import Grad Textures")),
					FSlateIcon()
				).SetCommandList(GStalkerEditorManager->UICommandList);
            })
        );


		Section.AddMenuEntry(StalkerEditorCommands::Get().ImportPhysicalMaterials,
			FText::FromString(TEXT("2. Import Physical Materials")),
			FText::FromString(TEXT("Import Physical Materials")),
			FSlateIcon()
		).SetCommandList(GStalkerEditorManager->UICommandList);

		Section.AddMenuEntry(StalkerEditorCommands::Get().ImportMeshes,
			FText::FromString(TEXT("3. Import Meshes")),
			FText::FromString(TEXT("Import Meshes")),
			FSlateIcon()
		).SetCommandList(GStalkerEditorManager->UICommandList);

		Section.AddMenuEntry(StalkerEditorCommands::Get().ImportParticles,
			FText::FromString(TEXT("4. Import Particles")),
			FText::FromString(TEXT("Import Particles")),
			FSlateIcon()
		).SetCommandList(GStalkerEditorManager->UICommandList);

    	Section.AddMenuEntry(StalkerEditorCommands::Get().ImportSounds,
			FText::FromString(TEXT("5. Import Sounds")),
			FText::FromString(TEXT("Import Sounds")),
			FSlateIcon()
		).SetCommandList(GStalkerEditorManager->UICommandList);

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
		Section.AddMenuEntry("AlwaysOverwriteAssetsWhenImport", FText::FromString(TEXT("Always overwrite assets")), FText::FromString(TEXT("Always overwrite assets when import")), FSlateIcon(), Action, EUserInterfaceActionType::Check);
	

	}
	else
	{
		UE_LOG(LogStalkerEditor, Error, TEXT("LevelEditor.MainMenu.Tools"))
	}
}

void FStalkerToolsMenu::Destroy()
{

}

