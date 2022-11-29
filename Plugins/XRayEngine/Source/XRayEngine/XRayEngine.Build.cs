// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class XRayEngine : ModuleRules
{
	public XRayEngine(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Private/XRayEnginePCH.h";

        PublicIncludePaths.AddRange(
			new string[] 
			{
            "Public"
            }
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
            "Private"
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Engine",
                "XRayEngineModules",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Slate",
                "SlateCore",
                "UMG",
                "InputCore"
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
