// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GASTPS : ModuleRules
{
	public GASTPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"EnhancedInput", "GameplayAbilities", "GameplayTasks","GameplayTags", "UMG", "Slate", "SlateCore",
			"AIModule", "Niagara", "OnlineSubsystem","OnlineSubsystemEOS","OnlineSubsystemUtils","Networking","HTTP","Json"
		});
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
