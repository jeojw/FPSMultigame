// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDatabaseManager.h"


UMyDatabaseManager::UMyDatabaseManager()
    : Database(nullptr)
{
}

UMyDatabaseManager::~UMyDatabaseManager()
{
    if (Database)
    {
        sqlite3_close(Database);
    }
}

bool UMyDatabaseManager::OpenDatabase(const FString& DatabasePath)
{
    FString FullPath = FPaths::ProjectDir() + DatabasePath;
    int32 Result = sqlite3_open(TCHAR_TO_UTF8(*FullPath), &Database);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to open database: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }
    return true;
    return true;
}

bool UMyDatabaseManager::CreateTable()
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLCreateTable = R"(
        CREATE TABLE IF NOT EXISTS PlayerData (
            MemberID TEXT PRIMARY KEY,
            MemberPW TEXT NOT NULL,
            MemberNickname TEXT NOT NULL
        );
    )";

    char* ErrorMessage = nullptr;
    int32 Result = sqlite3_exec(Database, SQLCreateTable, nullptr, nullptr, &ErrorMessage);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create table: %s"), UTF8_TO_TCHAR(ErrorMessage));
        sqlite3_free(ErrorMessage);
        return false;
    }
    return true;
}

bool UMyDatabaseManager::InsertPlayerData(const FString& MemberID, const FString& MemberPW, const FString& MemberNickname)
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLInsert = "INSERT INTO PlayerData (MemberID, MemberPW, MemberNickname) VALUES (?, ?, ?);";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLInsert, -1, &Statement, nullptr);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*MemberID), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(Statement, 2, TCHAR_TO_UTF8(*MemberPW), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(Statement, 3, TCHAR_TO_UTF8(*MemberNickname), -1, SQLITE_TRANSIENT);

    Result = sqlite3_step(Statement);
    sqlite3_finalize(Statement);

    if (Result != SQLITE_DONE)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to insert player data: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    return true;

    return true;
}

bool UMyDatabaseManager::GetPlayerData(const FString& MemberID, FString& OutMemberPW, FString& OutMemberNickname)
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLSelect = "SELECT MemberPW, MemberNickname FROM PlayerData WHERE MemberID = ?;";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLSelect, -1, &Statement, nullptr);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*MemberID), -1, SQLITE_TRANSIENT);

    Result = sqlite3_step(Statement);
    if (Result == SQLITE_ROW)
    {
        OutMemberPW = UTF8_TO_TCHAR(reinterpret_cast<const char*>(sqlite3_column_text(Statement, 0)));
        OutMemberNickname = UTF8_TO_TCHAR(reinterpret_cast<const char*>(sqlite3_column_text(Statement, 1)));
        sqlite3_finalize(Statement);
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get player data: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        sqlite3_finalize(Statement);
        return false;
    }

    return false;
}