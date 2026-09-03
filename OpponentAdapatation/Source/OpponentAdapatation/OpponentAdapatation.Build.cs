// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OpponentAdapatation : ModuleRules
{
	public OpponentAdapatation(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"OpponentAdapatation",
			"OpponentAdapatation/Variant_Platforming",
			"OpponentAdapatation/Variant_Platforming/Animation",
			"OpponentAdapatation/Variant_Combat",
			"OpponentAdapatation/Variant_Combat/AI",
			"OpponentAdapatation/Variant_Combat/Animation",
			"OpponentAdapatation/Variant_Combat/Gameplay",
			"OpponentAdapatation/Variant_Combat/Interfaces",
			"OpponentAdapatation/Variant_Combat/UI",
			"OpponentAdapatation/Variant_SideScrolling",
			"OpponentAdapatation/Variant_SideScrolling/AI",
			"OpponentAdapatation/Variant_SideScrolling/Gameplay",
			"OpponentAdapatation/Variant_SideScrolling/Interfaces",
			"OpponentAdapatation/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
