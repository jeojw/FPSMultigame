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

        string SSLIncludePath = Path.Combine(ThirdPartyPath, "OpenSSL", "include");
        string SSLiteLibPath = Path.Combine(ThirdPartyPath, "OpenSSL", "lib");
        string SSLiteBinPath = Path.Combine(ThirdPartyPath, "OpenSSL", "bin");

        PublicIncludePaths.AddRange(new string[] {
            SQLiteIncludePath,
            SSLIncludePath
        });


        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "NetCore",
                                                            "UMG", "OnlineSubsystem", "OnlineSubsystemUtils", "Landscape", "Steamworks", "Paper2D", 
                                                            "Slate", "SlateCore", "SQLiteSupport", "RHI","RenderCore","ImageWrapper" });


        PublicAdditionalLibraries.Add(Path.Combine(SQLiteLibPath, "sqlite3.lib"));

        RuntimeDependencies.Add(Path.Combine(SQLiteBinPath, "sqlite3.dll"));

        // Delay-load the DLL, so it's not required at link time
        PublicDelayLoadDLLs.Add("sqlite3.dll");


        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(SSLiteLibPath, "libcrypto.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(SSLiteLibPath, "libssl.lib"));
        }

        RuntimeDependencies.Add(Path.Combine(SSLiteBinPath, "libcrypto-1_1-x64.dll"));
        RuntimeDependencies.Add(Path.Combine(SSLiteBinPath, "libssl-1_1-x64.dll"));
    }
}
