// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DarkEdenSurvivorsTarget : TargetRules
{
	public DarkEdenSurvivorsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		//ExtraModuleNames.Add("DarkEdenSurvivors");
		ExtraModuleNames.AddRange(new string[] { "DarkEdenSurvivors", "DarkEdenSurvivorsSetting" });
	}
}
