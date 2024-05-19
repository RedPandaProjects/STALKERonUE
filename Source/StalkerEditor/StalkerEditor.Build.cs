using UnrealBuildTool;
using System.IO;

public class StalkerEditor : ModuleRules
{
	public StalkerEditor(ReadOnlyTargetRules Target) : base(Target)
    {

        CppStandard = CppStandardVersion.Cpp20;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "StalkerEditorPCH.h";
        PublicIncludePaths.Add(ModuleDirectory);
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
				"Engine",
				"Slate",
                "EditorStyle",
                "SlateCore",
                "Engine",
                "Slate",
                "SlateCore",
                "RenderCore",
                "Projects",
                "UnrealEd",
                "InputCore",
                "MeshDescription",
                "StaticMeshDescription",
                "MaterialEditor",
                "MaterialUtilities",
                "Stalker",
                "ToolMenus",
                "Landscape",
                "XRayEngineModules",
                "XRayEditorModules",
                "RedImage",
                "AnimGraph",
                "BlueprintGraph",
                "EditorFramework",
                "PlacementMode",
                "MessageLog",
                "Niagara", 
                "NiagaraCore",
                "NiagaraEditor",
                "Sequencer",
                "RHI",
                "Foliage",
                "VorbisAudioDecoder"
            });

        AddEngineThirdPartyPrivateStaticDependencies(Target,
            "UEOgg",
            "Vorbis",
            "VorbisFile"
        );
    }
}