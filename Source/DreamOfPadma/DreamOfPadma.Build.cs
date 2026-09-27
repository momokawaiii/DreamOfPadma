// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DreamOfPadma : ModuleRules
{
	public DreamOfPadma(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		// File-local helpers must remain isolated; UE 5.8 may group this entire module.
		bUseUnity = false;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", "GameplayTags", "GameplayTasks", "UMG", "CommonUI", "PCG" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "AnimGraphRuntime", "Niagara", "ProceduralMeshComponent", "LevelSequence", "MovieScene", "Json", "JsonUtilities" });

		if (Target.bBuildEditor) PrivateDependencyModuleNames.AddRange(new string[] { "NiagaraEditor", "UnrealEd", "AnimGraph", "BlueprintGraph", "AssetTools", "AssetRegistry" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
