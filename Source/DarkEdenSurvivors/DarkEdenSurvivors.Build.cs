// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class DarkEdenSurvivors : ModuleRules
{
	public DarkEdenSurvivors(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"UMG",
            "SlateCore",
            "Niagara",
			"Json",
            "JsonUtilities",
            "GameplayTags"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {"DarkEdenSurvivorsSetting" });

		PublicIncludePaths.AddRange(new string[] {
			"DarkEdenSurvivors",
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
