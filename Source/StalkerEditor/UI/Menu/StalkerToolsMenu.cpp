#include "StalkerToolsMenu.h"
#include "../Commands/StalkerEditorCommands.h"
#include "../StalkerEditorStyle.h"
#include "../../StalkerEditorManager.h"

void StalkerToolsMenu::Initialize()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	if (UToolMenu* ProfileMenu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Tools")))
	{
		FToolMenuSection& Section = ProfileMenu->AddSection("Stalker Tools", FText::FromString(TEXT("Stalker Tools")));
		Section.AddMenuEntry(StalkerEditorCommands::Get().ImportUITextures,
			FText::FromString(TEXT("Import UI Textures")),
			FText::FromString(TEXT("Import UI Textures")),
			FSlateIcon()
		).SetCommandList(GStalkerEditorManager->UICommandList);

		Section.AddMenuEntry(StalkerEditorCommands::Get().ImportPhysicalMaterials,
			FText::FromString(TEXT("Import Physical Materials")),
			FText::FromString(TEXT("Import Physical Materials")),
			FSlateIcon()
		).SetCommandList(GStalkerEditorManager->UICommandList);

		Section.AddMenuEntry(StalkerEditorCommands::Get().ImportMeshes,
			FText::FromString(TEXT("Import Meshes")),
			FText::FromString(TEXT("Import Meshes")),
			FSlateIcon()
		).SetCommandList(GStalkerEditorManager->UICommandList);

		Section.AddMenuEntry(StalkerEditorCommands::Get().ImportParticles,
			FText::FromString(TEXT("Import Particles")),
			FText::FromString(TEXT("Import Particles")),
			FSlateIcon()
		).SetCommandList(GStalkerEditorManager->UICommandList);

	}
	else
	{
		UE_LOG(LogStalkerEditor, Error, TEXT("LevelEditor.MainMenu.Tools"))
	}
}

void StalkerToolsMenu::Destroy()
{

}

