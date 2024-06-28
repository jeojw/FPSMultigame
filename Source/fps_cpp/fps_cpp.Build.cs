// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class fps_cpp : ModuleRules
{
    public fps_cpp(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        string ProjectRoot = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../"));
        string ThirdPartyPath = Path.Combine(ProjectRoot, "Source/ThirdParty");
        string SQLiteIncludePath = Path.Combine(ThirdPartyPath, "SQLite", "include");
        string SQLiteLibPath = Path.Combine(ThirdPartyPath, "SQLite", "lib");
        string SQLiteBinPath = Path.Combine(ThirdPartyPath, "SQLite", "bin");

        PublicIncludePaths.AddRange(new string[] {
            SQLiteIncludePath
        });

        PublicIncludePaths.AddRange(new string[] {
            Path.Combine(ModuleDirectory, "ThirdParty", "SQLite", "include")
        });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "NetCore",
            "UMG", "OnlineSubsystem", "OnlineSubsystemUtils", "Landscape", "Steamworks", "Paper2D", "Slate", "SlateCore", "SQLiteSupport" });


        PublicAdditionalLibraries.Add(Path.Combine(SQLiteLibPath, "sqlite3.lib"));

        // 프로젝트 내의 DLL 파일 경로를 설정합니다.
        RuntimeDependencies.Add(Path.Combine(SQLiteBinPath, "sqlite3.dll"));

        // Delay-load the DLL, so it's not required at link time
        PublicDelayLoadDLLs.Add("sqlite3.dll");
    }
}
