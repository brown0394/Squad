// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Squad : ModuleRules
{
	public Squad(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTasks" });
	}
}
