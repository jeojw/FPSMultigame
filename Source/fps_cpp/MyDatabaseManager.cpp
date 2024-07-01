// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDatabaseManager.h"
#include "Misc/Paths.h"
#include <openssl/sha.h>
#include <string>

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
    FString FullPath = FPaths::Combine(FPaths::ProjectContentDir(), DatabasePath);
    int32 Result = sqlite3_open(TCHAR_TO_UTF8(*FullPath), &Database);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to open database: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }
    return true;
}

bool UMyDatabaseManager::ExecuteSQLFile(const FString& SQLFilePath)
{
    FString FullPath = FPaths::Combine(FPaths::ProjectContentDir(), SQLFilePath);
    FString FileContent;

    if (!FFileHelper::LoadFileToString(FileContent, *FullPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load SQL file: %s"), *FullPath);
        return false;
    }

    char* ErrorMessage = nullptr;
    int32 Result = sqlite3_exec(Database, TCHAR_TO_UTF8(*FileContent), nullptr, nullptr, &ErrorMessage);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to execute SQL file: %s"), UTF8_TO_TCHAR(ErrorMessage));
        sqlite3_free(ErrorMessage);
        return false;
    }

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

bool UMyDatabaseManager::CheckDuplicateID(const FString& MemberID)
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLCheckIdDuplicate = R"(
        SELECT EXISTS (
            SELECT * FROM PlayerData
            WHERE PlayerID = ?
        );
    )";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLCheckIdDuplicate, -1, &Statement, nullptr);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*MemberID), -1, SQLITE_TRANSIENT);

    Result = sqlite3_step(Statement);

    bool bExists = false;
    if (Result == SQLITE_ROW)
    {
        bExists = sqlite3_column_int(Statement, 0) == 1;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to check for duplicate ID: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
    }

    sqlite3_finalize(Statement);

    return bExists;
}

bool UMyDatabaseManager::CheckDuplicateNickname(const FString& MemberNickname)
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLCheckIdDuplicate = R"(
        SELECT EXISTS (
            SELECT * FROM PlayerData
            WHERE PlayerNickname = ?
        );
    )";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLCheckIdDuplicate, -1, &Statement, nullptr);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*MemberNickname), -1, SQLITE_TRANSIENT);

    Result = sqlite3_step(Statement);

    bool bExists = false;
    if (Result == SQLITE_ROW)
    {
        bExists = sqlite3_column_int(Statement, 0) == 1;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to check for duplicate ID: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
    }

    sqlite3_finalize(Statement);

    return bExists;
}

bool UMyDatabaseManager::AddPlayerToRoom(int32 RoomID, int32 PlayerID)
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLInsertRoomUser = "INSERT INTO RoomPlayers (RoomID, PlayerID) VALUES (?, ?);";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLInsertRoomUser, -1, &Statement, nullptr);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    sqlite3_bind_int(Statement, 1, RoomID);
    sqlite3_bind_int(Statement, 2, PlayerID);

    Result = sqlite3_step(Statement);
    sqlite3_finalize(Statement);

    if (Result != SQLITE_DONE)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to add user to room: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    return true;
}

UFUNCTION(BlueprintCallable, Category = "Database")
bool UMyDatabaseManager::DeletePlayerToRoom(int32 RoomID, int32 PlayerID)
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLInsertRoomUser = "DELETE FROM RoomPlayers WHERE RoomID =? AND PlayerID = ?;";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLInsertRoomUser, -1, &Statement, nullptr);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    sqlite3_bind_int(Statement, 1, RoomID);
    sqlite3_bind_int(Statement, 2, PlayerID);

    Result = sqlite3_step(Statement);
    sqlite3_finalize(Statement);

    if (Result != SQLITE_DONE)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to remove user from room: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    return true;
}

TArray<int32> UMyDatabaseManager::GetPlayersInRoom(int32 RoomID)
{
    TArray<int32> PlayerIDs;

    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return PlayerIDs;
    }

    const char* SQLSelectUsersInRoom = "SELECT PlayerID FROM RoomPlayers WHERE RoomID = ?;";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLSelectUsersInRoom, -1, &Statement, nullptr);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return PlayerIDs;
    }

    sqlite3_bind_int(Statement, 1, RoomID);

    while (sqlite3_step(Statement) == SQLITE_ROW)
    {
        int32 PlayerID = sqlite3_column_int(Statement, 0);
        PlayerIDs.Add(PlayerID);
    }

    sqlite3_finalize(Statement);
    return PlayerIDs;
}

bool UMyDatabaseManager::InsertPlayerData(const FString& MemberID, const FString& MemberPW, const FString& MemberNickname)
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLInsert = "INSERT INTO PlayerData (PlayerID, PlayerPW, PlayerNickname) VALUES (?, ?, ?);";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLInsert, -1, &Statement, nullptr);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    std::string PasswordStr = TCHAR_TO_UTF8(*MemberPW);

    unsigned char Hash[SHA256_DIGEST_LENGTH];

    SHA256((unsigned char*)PasswordStr.c_str(), PasswordStr.length(), Hash);

    FString HashString;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        HashString += FString::Printf(TEXT("%02x"), Hash[i]);
    }

    sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*MemberID), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(Statement, 2, TCHAR_TO_UTF8(*HashString), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(Statement, 3, TCHAR_TO_UTF8(*MemberNickname), -1, SQLITE_TRANSIENT);

    Result = sqlite3_step(Statement);
    sqlite3_finalize(Statement);

    if (Result != SQLITE_DONE)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to insert player data: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    return true;
}

bool UMyDatabaseManager::CheckPlayerData(const FString& MemberID, const FString& MemberPW)
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLCheckLogin = R"(
        SELECT EXISTS (
            SELECT 1 FROM PlayerData
            WHERE PlayerID = ? AND PlayerPW = ?
        );
    )";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLCheckLogin, -1, &Statement, nullptr);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    std::string PasswordStr = TCHAR_TO_UTF8(*MemberPW);

    unsigned char Hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)PasswordStr.c_str(), PasswordStr.length(), Hash);

    FString HashString;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        HashString += FString::Printf(TEXT("%02x"), Hash[i]);
    }

    sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*MemberID), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(Statement, 2, TCHAR_TO_UTF8(*HashString), -1, SQLITE_TRANSIENT);

    bool bExists = false;
    Result = sqlite3_step(Statement);

    if (Result == SQLITE_ROW)
    {
        bExists = sqlite3_column_int(Statement, 0) == 1;
    }
    else if (Result != SQLITE_DONE)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to check player: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
    }

    sqlite3_finalize(Statement);

    return bExists;
}

bool UMyDatabaseManager::LogInPlayer(const FString& PlayerID)
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    // �̹� �α��ε� �������� Ȯ��
    const char* SQLCheckLogin = R"(
        SELECT EXISTS (
            SELECT 1 FROM LoggedInPlayers WHERE PlayerID = ?
        );
    )";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLCheckLogin, -1, &Statement, nullptr);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*PlayerID), -1, SQLITE_TRANSIENT);

    Result = sqlite3_step(Statement);
    bool bExists = sqlite3_column_int(Statement, 0) == 1;
    sqlite3_finalize(Statement);

    if (bExists)
    {
        UE_LOG(LogTemp, Warning, TEXT("User is already logged in."));
        return false;
    }

    // �α��� ó��
    const char* SQLInsertSession = R"(
        INSERT INTO LoggedInPlayers (PlayerID) VALUES (?);
    )";
    Result = sqlite3_prepare_v2(Database, SQLInsertSession, -1, &Statement, nullptr);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*PlayerID), -1, SQLITE_TRANSIENT);

    Result = sqlite3_step(Statement);
    sqlite3_finalize(Statement);

    if (Result != SQLITE_DONE)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to insert session: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    return true;
}

bool UMyDatabaseManager::LogOutPlayer(const FString& PlayerID)
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLDeleteSession = R"(
        DELETE FROM LoggedInPlayers WHERE PlayerID = ?;
    )";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLDeleteSession, -1, &Statement, nullptr);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*PlayerID), -1, SQLITE_TRANSIENT);

    Result = sqlite3_step(Statement);
    sqlite3_finalize(Statement);

    if (Result != SQLITE_DONE)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to delete session: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

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
}