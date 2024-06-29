// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "fps_cppPlayerState.h"
#include "fps_cppCharacter.h"
#include "fps_cppPlayerController.h"
#include "Sound/SoundWave.h"
#include "GameFramework/GameModeBase.h"
#include "Components/AudioComponent.h"
#include "MyDatabaseManager.h"
#include "fps_cppGameMode.generated.h"

UCLASS(minimalapi)
class Afps_cppGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY()
	float RespawnTime;

	UPROPERTY()
	UMyDatabaseManager* DatabaseManager;

	void InitializeDatabase();
	void LoadPlayerData(const FString& MemberID);
	void SavePlayerData(const FString& MemberID, const FString& MemberPW, const FString& MemberNickname);

public:
	Afps_cppGameMode();

	void StartGame();
	
	void Respawn(Afps_cppPlayerController* PlayerController, Afps_cppPlayerState* PlayerState);
	void RespawnFunction(Afps_cppPlayerController* PlayerController, Afps_cppPlayerState* PlayerState);

	void CreateDefaultSaveGame();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

private:
	void InitializeNetworkSettings();

	TMap<Afps_cppPlayerState*, bool> PlayerRespawnRequested;

	TMap<Afps_cppPlayerState*, FTimerHandle> PlayerRespawnTimers;

	UPROPERTY(EditAnywhere)
	USoundWave* StartBGM;

	UPROPERTY(EditAnywhere)
	USoundWave* GameRoomBGM;

	UPROPERTY()
	UAudioComponent* StartBGMAudioComponent;
};



