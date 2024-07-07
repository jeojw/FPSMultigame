// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "sqlite3.h"
#include "MyDatabaseManager.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API UMyDatabaseManager : public UObject
{
	GENERATED_BODY()

public:
    UMyDatabaseManager();
    ~UMyDatabaseManager();

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool OpenDatabase(const FString& DatabasePath);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool ExecuteSQLFile(const FString& SQLFilePath);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool CreateTable();

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool CheckDuplicateID(const FString& MemberID);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool CheckDuplicateNickname(const FString& MemberNickname);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool InsertPlayerData(const FString& MemberID, const FString& MemberPW, const FString& MemberNickname);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool CheckPlayerData(const FString& MemberID, const FString& MemberPW);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool LogInPlayer(const FString& PlayerID);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool LogOutPlayer(const FString& PlayerID);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool GetPlayerData(const FString& MemberID, FString& OutMemberNickname, FString& OutProfileImagePath);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool AddPlayerToRoom(int32 RoomID, int32 PlayerID);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool DeletePlayerToRoom(int32 RoomID, int32 PlayerID);

    UFUNCTION(BlueprintCallable, Category = "Database")
    TArray<int32> GetPlayersInRoom(int32 RoomID);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool ChangeProfileImage(const TArray<uint8>& ImageData, const FString& PlayerID, const FString& ImageName);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool CreateRoom(const FString& RoomTitle, const FString& RoomManager, const int32& maxPlayers, const int32& isExistLock, const FString& RoomPassword);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool DeleteRoom(const FString& RoomID);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool ChangeRoom(const FString& RoomID, const FString& RoomTitle, const int32& maxPlayers, const int32& isExistLock, const FString& RoomPassword);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool ChangeManager(const FString& RoomID);

private:
    sqlite3* Database;
};
