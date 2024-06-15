// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class fps_cpp : ModuleRules
{
    public fps_cpp(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "NetCore", 
            "UMG", "OnlineSubsystem", "OnlineSubsystemUtils", "Landscape", "Steamworks", "Paper2D", "Slate", "SlateCore" });
    }
}
