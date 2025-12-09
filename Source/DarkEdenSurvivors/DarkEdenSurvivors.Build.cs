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
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"NavigationSystem",
			"Slate",
			"GameplayTasks",
            "Niagara"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {"DarkEdenSurvivorsSetting" });

		PublicIncludePaths.AddRange(new string[] {
			"DarkEdenSurvivors",
			"DarkEdenSurvivors/Variant_Platforming",
			"DarkEdenSurvivors/Variant_Platforming/Animation",
			"DarkEdenSurvivors/Variant_Combat",
			"DarkEdenSurvivors/Variant_Combat/AI",
			"DarkEdenSurvivors/Variant_Combat/Animation",
			"DarkEdenSurvivors/Variant_Combat/Gameplay",
			"DarkEdenSurvivors/Variant_Combat/Interfaces",
			"DarkEdenSurvivors/Variant_Combat/UI",
			"DarkEdenSurvivors/Variant_SideScrolling",
			"DarkEdenSurvivors/Variant_SideScrolling/AI",
			"DarkEdenSurvivors/Variant_SideScrolling/Gameplay",
			"DarkEdenSurvivors/Variant_SideScrolling/Interfaces",
			"DarkEdenSurvivors/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
