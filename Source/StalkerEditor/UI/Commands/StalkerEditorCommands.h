#pragma once
class StalkerEditorCommands : public TCommands<StalkerEditorCommands>
{
public:

	StalkerEditorCommands();
	void RegisterCommands() override;
public:
	TSharedPtr< FUICommandInfo > BuildCForm;
	TSharedPtr< FUICommandInfo > BuildAIMap;
	TSharedPtr< FUICommandInfo > BuildLevelSpawn;
	TSharedPtr< FUICommandInfo > BuildGameSpawn;
	TSharedPtr< FUICommandInfo > ReloadConfigsAndScript;
	TSharedPtr< FUICommandInfo > ImportAllTextures;
	TSharedPtr< FUICommandInfo > ImportUITextures;
	TSharedPtr< FUICommandInfo > ImportWMTextures;
	TSharedPtr< FUICommandInfo > ImportGradTextures;
	TSharedPtr< FUICommandInfo > ImportMeshes;
	TSharedPtr< FUICommandInfo > ImportPhysicalMaterials;
	TSharedPtr< FUICommandInfo > ImportParticles;
	TSharedPtr< FUICommandInfo > ImportSounds;
};