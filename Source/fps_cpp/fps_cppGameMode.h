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

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PlayerUIWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> StartWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> LoginWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> SignupWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> OptionWidgetClass;

public:
	Afps_cppGameMode();

	void StartGame();
	void VisibleSignup();
	void VisibleLogin();
	void VisibleOption();
	void VisiblePlayerUI();

	void PauseGame();
	void UnpauseGame();
	
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
	UUserWidget* PlayerUIWidgetInstance;

	UPROPERTY(EditAnywhere)
	UUserWidget* SignupWidgetInstance;

	UPROPERTY(EditAnywhere)
	UUserWidget* LoginWidgetInstance;

	UPROPERTY(EditAnywhere)
	UUserWidget* StartWidgetInstance;

	UPROPERTY(EditAnywhere)
	UUserWidget* OptionWidgetInstance;

	UPROPERTY(EditAnywhere)
	USoundWave* StartBGM;

	UPROPERTY(EditAnywhere)
	USoundWave* GameRoomBGM;

	UPROPERTY()
	UAudioComponent* StartBGMAudioComponent;

	FTimerHandle RespawnTimerHandle;
};



