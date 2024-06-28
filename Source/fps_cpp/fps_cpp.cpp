// Copyright Epic Games, Inc. All Rights Reserved.

#include "fps_cpp.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "sqlite3.h"

IMPLEMENT_PRIMARY_GAME_MODULE(Ffps_cppModule, fps_cpp, "fps_cpp");

void Ffps_cppModule::StartupModule()
{
    FString DLLPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Source/ThirdParty/SQLite/bin/sqlite3.dll"));

    SQLiteDLLHandle = FPlatformProcess::GetDllHandle(*DLLPath);

    if (!SQLiteDLLHandle)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load sqlite3.dll from %s"), *DLLPath);
    }
}

void Ffps_cppModule::ShutdownModule()
{
    if (SQLiteDLLHandle)
    {
        FPlatformProcess::FreeDllHandle(SQLiteDLLHandle);
        SQLiteDLLHandle = nullptr;
    }
}