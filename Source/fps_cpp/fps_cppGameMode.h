// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "fps_cppCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "fps_cppGameMode.generated.h"

UCLASS(minimalapi)
class Afps_cppGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY()
	float RespawnTime;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> StartWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> LoginWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> SignupWidgetClass;

public:
	Afps_cppGameMode();

	//void Login();
	//void Signup();
	void StartGame();
	
	
	void Respawn();
	void RespawnFunction();

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
	UUserWidget* CurrentWidget;

	FTimerHandle RespawnTimerHandle;
};



