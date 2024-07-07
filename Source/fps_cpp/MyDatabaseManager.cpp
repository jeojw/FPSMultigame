// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDatabaseManager.h"
#include "Misc/Paths.h"
#include "Misc/Guid.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
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
    FString FullPath = FPaths::Combine(FPaths::ProjectSavedDir(), DatabasePath);
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
    FString FullPath = FPaths::Combine(FPaths::ProjectSavedDir(), SQLFilePath);
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

    const char* SQLInsert = "INSERT INTO PlayerData (PlayerID, PlayerPW, PlayerNickname, PlayerProfileImage) VALUES (?, ?, ?, ?);";
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

    UE_LOG(LogTemp, Log, TEXT("MemberID: %s"), *MemberID);
    UE_LOG(LogTemp, Log, TEXT("HashString: %s"), *HashString);
    UE_LOG(LogTemp, Log, TEXT("MemberNickname: %s"), *MemberNickname);
    FString SaveDirectory = FPaths::ProjectSavedDir() + TEXT("ProfileImages/Basic_Profile.png");
    UE_LOG(LogTemp, Log, TEXT("SaveDirectory: %s"), *SaveDirectory);

    sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*MemberID), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(Statement, 2, TCHAR_TO_UTF8(*HashString), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(Statement, 3, TCHAR_TO_UTF8(*MemberNickname), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(Statement, 4, TCHAR_TO_UTF8(*SaveDirectory), -1, SQLITE_TRANSIENT);

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

bool UMyDatabaseManager::GetPlayerData(const FString& MemberID, FString& OutMemberNickname, FString& OutProfileImagePath)
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLSelect = "SELECT PlayerNickname, PlayerProfileImage FROM PlayerData WHERE PlayerID = ?;";
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
        OutMemberNickname = UTF8_TO_TCHAR(reinterpret_cast<const char*>(sqlite3_column_text(Statement, 0)));
        OutProfileImagePath = UTF8_TO_TCHAR(reinterpret_cast<const char*>(sqlite3_column_text(Statement, 1)));
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

bool UMyDatabaseManager::ChangeProfileImage(const TArray<uint8>& ImageData, const FString& PlayerID, const FString& ImageName)
{
    FString SaveDirectory = FPaths::ProjectSavedDir() + TEXT("ProfileImages/");
    FString FilePath = SaveDirectory + PlayerID + "_" + ImageName;

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    if (!PlatformFile.DirectoryExists(*SaveDirectory))
    {
        PlatformFile.CreateDirectory(*SaveDirectory);
        if (!PlatformFile.DirectoryExists(*SaveDirectory))
        {
            return false;
        }
    }

    if (FFileHelper::SaveArrayToFile(ImageData, *FilePath))
    {
        if (!Database)
        {
            UE_LOG(LogTemp, Error, TEXT("Database is not open."));
            return false;
        }
        const char* SQLSelect = "UPDATE PlayerData SET PlayerProfileImage = ? WHERE PlayerID = ?;";
        sqlite3_stmt* Statement;
        int32 Result = sqlite3_prepare_v2(Database, SQLSelect, -1, &Statement, nullptr);

        if (Result != SQLITE_OK)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
            return false;
        }

        sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*FilePath), -1, SQLITE_STATIC);
        sqlite3_bind_text(Statement, 2, TCHAR_TO_UTF8(*PlayerID), -1, SQLITE_STATIC);

        Result = sqlite3_step(Statement);
        if (Result == SQLITE_ROW)
        {
            sqlite3_finalize(Statement);
            return true;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to change profile image: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
            sqlite3_finalize(Statement);
            return false;
        }
    }

    return false;
}

bool UMyDatabaseManager::CreateRoom(const FString& RoomTitle, const FString& RoomManager, const int32& maxPlayers, const int32& isExistLock, const FString& RoomPassword)
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLSelect = "INSERT INTO RoomData (RoomID, RoomTitle, IsExistLock, MaxParticipates, Manager, RoomPW) VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLSelect, -1, &Statement, nullptr);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }

    FGuid RoomId = FGuid::NewGuid();
    FString RoomIdString = RoomId.ToString(EGuidFormats::DigitsWithHyphens);

    sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*RoomIdString), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(Statement, 2, TCHAR_TO_UTF8(*RoomTitle), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(Statement, 3, isExistLock);
    sqlite3_bind_int(Statement, 4, maxPlayers);
    sqlite3_bind_text(Statement, 5, TCHAR_TO_UTF8(*RoomManager), -1, SQLITE_TRANSIENT);
    if (isExistLock != 0)
    {
        sqlite3_bind_text(Statement, 6, TCHAR_TO_UTF8(*RoomPassword), -1, SQLITE_TRANSIENT);
    }
    else
    {
        sqlite3_bind_null(Statement, 6);
    }

    Result = sqlite3_step(Statement);
    if (Result == SQLITE_ROW)
    {
        sqlite3_finalize(Statement);
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create room: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        sqlite3_finalize(Statement);
        return false;
    }
}

bool UMyDatabaseManager::DeleteRoom(const FString& RoomID)
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLSelect = "DELETE FROM RoomData WHERE RoomID = ?;";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLSelect, -1, &Statement, nullptr);

    sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*RoomID), -1, SQLITE_TRANSIENT);

    Result = sqlite3_step(Statement);
    if (Result == SQLITE_ROW)
    {
        sqlite3_finalize(Statement);
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to delete room: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        sqlite3_finalize(Statement);
        return false;
    }
}

bool UMyDatabaseManager::ChangeRoom(const FString& RoomID, const FString& RoomTitle, const int32& maxPlayers, const int32& isExistLock, const FString& RoomPassword)
{
    if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLSelect = "UPDATE RoomData SET RoomTitle = ? AND MaxParticipates = ? AND IsExistLock = ? AND RoomPW = ? WHERE RoomID = ?;";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLSelect, -1, &Statement, nullptr);

    if (Result != SQLITE_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        return false;
    }
  
    sqlite3_bind_text(Statement, 1, TCHAR_TO_UTF8(*RoomTitle), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(Statement, 2, maxPlayers);
    sqlite3_bind_int(Statement, 3, isExistLock);
    if (isExistLock != 0)
    {
        sqlite3_bind_text(Statement, 4, TCHAR_TO_UTF8(*RoomPassword), -1, SQLITE_TRANSIENT);
    }
    else
    {
        sqlite3_bind_null(Statement, 4);
    }
    sqlite3_bind_text(Statement, 5, TCHAR_TO_UTF8(*RoomID), -1, SQLITE_TRANSIENT);

    Result = sqlite3_step(Statement);
    if (Result == SQLITE_ROW)
    {
        sqlite3_finalize(Statement);
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to change room: %s"), UTF8_TO_TCHAR(sqlite3_errmsg(Database)));
        sqlite3_finalize(Statement);
        return false;
    }
}

bool UMyDatabaseManager::ChangeManager(const FString& RoomID)
{
    /*if (!Database)
    {
        UE_LOG(LogTemp, Error, TEXT("Database is not open."));
        return false;
    }

    const char* SQLSelect = "UPDATE RoomData SET RoomTitle = ? AND MaxParticipates = ? AND IsExistLock = ? AND RoomPW = ? WHERE RoomID = ?;";
    sqlite3_stmt* Statement;
    int32 Result = sqlite3_prepare_v2(Database, SQLSelect, -1, &Statement, nullptr);*/

    return true;
}