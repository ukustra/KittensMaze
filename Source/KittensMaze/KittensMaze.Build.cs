// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KittensMaze : ModuleRules
{
	public KittensMaze(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "MoviePlayer", "SlateCore", "GameplayAbilities", "GameplayTasks", "GameplayTags", "DeveloperSettings", "Voxel", "SurfaceFootstepSystem" });
	}
}
