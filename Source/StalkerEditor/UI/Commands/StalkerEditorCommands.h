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
	TSharedPtr< FUICommandInfo > ImportUITextures;
	TSharedPtr< FUICommandInfo > ImportMeshes;
	TSharedPtr< FUICommandInfo > ImportPhysicalMaterials;
	TSharedPtr< FUICommandInfo > ImportParticles;
};