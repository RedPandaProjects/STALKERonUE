// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class StalkerEditorTarget : TargetRules
{
	public StalkerEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		ExtraModuleNames.AddRange( new string[] { "Stalker" } );
		ExtraModuleNames.AddRange( new string[] { "StalkerEditor" } );
		ExtraModuleNames.AddRange(new string[] { "StalkerLoading" });
	}
}
