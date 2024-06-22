// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "fps_cppCharacter.h"
#include "Sound/SoundWave.h"
#include "GameFramework/GameModeBase.h"
#include "Components/AudioComponent.h"
#include "fps_cppGameMode.generated.h"

UCLASS(minimalapi)
class Afps_cppGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY()
	float RespawnTime;

public:
	Afps_cppGameMode();

	void StartGame();
	
	void Respawn();
	void RespawnFunction();

	void CreateDefaultSaveGame();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

private:
	void InitializeNetworkSettings();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class Afps_cppCharacter> PlayerPawnBPClass;

	UPROPERTY(EditAnywhere)
	Afps_cppCharacter* Player;

	UPROPERTY(EditAnywhere)
	USoundWave* StartBGM;

	UPROPERTY(EditAnywhere)
	USoundWave* GameRoomBGM;

	UPROPERTY()
	UAudioComponent* StartBGMAudioComponent;

	FTimerHandle RespawnTimerHandle;
};



