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

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "../ThirdParty/MySQL/include"),
                Path.Combine(ModuleDirectory, "../ThirdParty/MySQL/include/jdbc"),
                Path.Combine(ModuleDirectory, "../ThirdParty/MySQL/include/mysql"),
                Path.Combine(ModuleDirectory, "../ThirdParty/MySQL/include/mysqlx")
            }
        );

        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/MySQL/lib/mysqlcppconn8.lib"));
    }
}
