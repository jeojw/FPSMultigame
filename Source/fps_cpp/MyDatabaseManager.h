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
    bool CreateTable();

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool InsertPlayerData(const FString& MemberID, const FString& MemberPW, const FString& MemberNickname);

    UFUNCTION(BlueprintCallable, Category = "Database")
    bool GetPlayerData(const FString& MemberID, FString& OutMemberPW, FString& OutMemberNickname);

private:
    sqlite3* Database;
};
