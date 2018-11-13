// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HotStuff : ModuleRules
{
	public HotStuff(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemSteam", "PhysXVehicles", "HeadMountedDisplay", "UMG" });

		PublicDefinitions.Add("HMD_MODULE_INCLUDED=1");
	}
}
