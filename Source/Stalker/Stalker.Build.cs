// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Stalker : ModuleRules
{
	public Stalker(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "StalkerPCH.h";

        PublicIncludePaths.Add(ModuleDirectory);

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore","UMG","XRayEngineModules", "Slate", "AnimGraph","BlueprintGraph", "SlateCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
