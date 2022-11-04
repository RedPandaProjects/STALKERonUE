// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class XRayImporter : ModuleRules
{
	public XRayImporter(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Private/XRayImporterPCH.h";

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
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
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
                "XRayEngine",
                "XRayEngineModules",
                "XRayEditorModules",
                "RedImage",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
