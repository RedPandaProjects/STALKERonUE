using UnrealBuildTool;
using System.IO;

public class StalkerEditor : ModuleRules
{
	public StalkerEditor(ReadOnlyTargetRules Target) : base(Target)
    {

        CppStandard = CppStandardVersion.Cpp20;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "StalkerEditorPCH.h";

        PublicIncludePaths.AddRange(
            new string[] {
					// ... add public include paths required here ...
            }
            );

        PrivateIncludePaths.AddRange(
            new string[] {
                // ... add other private include paths required here ...
            }
            );

        PublicDependencyModuleNames.AddRange(
            new string[] 
            {
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
				"Engine",
				"Slate",
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
                "XRayEngineModules",
                "XRayEditorModules",
                "RedImage", "AnimGraph","BlueprintGraph",
                // ... add private dependencies that you statically link with here ...
            }
            );
    }
}