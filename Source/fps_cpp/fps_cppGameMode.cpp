// Copyright Epic Games, Inc. All Rights Reserved.

#include "fps_cppGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "SaveOptions.h"
#include "fps_cppPlayerController.h"

Afps_cppGameMode::Afps_cppGameMode() : AGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerControllerClass = Afps_cppPlayerController::StaticClass();

	PlayerStateClass = Afps_cppPlayerState::StaticClass();

	DefaultPawnClass = Afps_cppCharacter::StaticClass();

	static ConstructorHelpers::FObjectFinder<USoundWave> StartBGMFinder(TEXT("/Game/FPS_BGM/Phat_Phrog_Studio_-_Dropship_Assault_-_Uprising_Protocol_-_LOOP"));
	if (StartBGMFinder.Succeeded())
	{
		StartBGM = StartBGMFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> GameRoomBGMFinder(TEXT("/Game/FPS_BGM/Phat_Phrog_Studio_-_Mecha_Conflict_-_Ironblood_Frontier_-_LOOP"));
	if (GameRoomBGMFinder.Succeeded())
	{
		GameRoomBGM = GameRoomBGMFinder.Object;
	}
	
	RespawnTime = 1.0f;
}

void Afps_cppGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!UGameplayStatics::DoesSaveGameExist(TEXT("SettingsSlot"), 0))
	{
		CreateDefaultSaveGame();
	}

	if (StartBGM)
	{
		USaveOptions* LoadOptionsInstance = Cast<USaveOptions>(UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSlot"), 0));

		//StartBGMAudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), StartBGM, 1.0f, 1.0f, 0.0f, nullptr, true, true);
	}

	InitializeNetworkSettings();
}

void Afps_cppGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* Controller = It->Get();
		Afps_cppPlayerController* MyPlayerController = Cast<Afps_cppPlayerController>(Controller);

		if (MyPlayerController)
		{
			Afps_cppPlayerState* PlayerState = MyPlayerController->GetPlayerState<Afps_cppPlayerState>();

			if (PlayerState && PlayerState->GetIsDead())
			{
				UE_LOG(LogTemp, Log, TEXT("Respawn!!"));
				RespawnFunction(MyPlayerController, PlayerState);
			}
		}
	}
}

void Afps_cppGameMode::CreateDefaultSaveGame()
{
	USaveOptions* SaveOptionsInstance = Cast<USaveOptions>(UGameplayStatics::CreateSaveGameObject(USaveOptions::StaticClass()));
	if (SaveOptionsInstance)
	{
		SaveOptionsInstance->SetMasterVolume(0.7f);
		SaveOptionsInstance->SetEffectVolume(1.0f);
		SaveOptionsInstance->SetMusicVolume(1.0f);
		SaveOptionsInstance->SetTextureQuality(TEXT("High"));

		UGameplayStatics::SaveGameToSlot(SaveOptionsInstance, TEXT("SettingsSlot"), 0);
	}
}

void Afps_cppGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	Afps_cppPlayerController* MyPlayerController = Cast<Afps_cppPlayerController>(NewPlayer);
	if (MyPlayerController)
	{
		MyPlayerController->ClientInitializeUI();

		Afps_cppCharacter* Player = Cast<Afps_cppCharacter>(MyPlayerController->GetPawn());
		Afps_cppPlayerState* PlayerState = MyPlayerController->GetPlayerState<Afps_cppPlayerState>();

		if (Player && PlayerState)
		{
			Player->InitializePlayerState();
			UE_LOG(LogTemp, Log, TEXT("PlayerState initialized successfully in GameMode."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Player or PlayerState is null in GameMode PostLogin."));
		}
	}

	//FVector SpawnLocation(1000.0f, 600.0f, 0.0f);
	//FRotator SpawnRotation(0.0f, 0.0f, 0.0f);

	//// 스폰 매개변수 설정
	//FActorSpawnParameters SpawnParams;
	//SpawnParams.Owner = this;
	//SpawnParams.Instigator = GetInstigator();

	//APawn* NewCharacter = GetWorld()->SpawnActor<APawn>(PlayerPawnBPClass, SpawnLocation, SpawnRotation, SpawnParams);
	//if (NewCharacter)
	//{
	//	NewPlayer->Possess(NewCharacter);
	//}
}

void Afps_cppGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void Afps_cppGameMode::Respawn()
{
	/*if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &Afps_cppGameMode::RespawnFunction, RespawnTime, false);
	}*/
}

void Afps_cppGameMode::RespawnFunction(Afps_cppPlayerController* PlayerController, Afps_cppPlayerState* PlayerState)
{
	if (PlayerController && PlayerState)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FVector SpawnLocation(900.0f, 1100.0f, 92.0f);
			FRotator SpawnRotation = FRotator::ZeroRotator;
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = PlayerController;

			Afps_cppCharacter* NewPawn = World->SpawnActor<Afps_cppCharacter>(DefaultPawnClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (NewPawn)
			{
				PlayerController->Possess(NewPawn);
				PlayerState->SetIsDead(false); // PlayerState의 IsDead 상태를 리셋
				UE_LOG(LogTemp, Log, TEXT("Player has been respawned and possessed by the controller"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to spawn new pawn"));
			}
		}
	}
}

void Afps_cppGameMode::InitializeNetworkSettings()
{
	// 네트워크 설정 초기화 (필요한 경우 추가 설정)
	UE_LOG(LogTemp, Warning, TEXT("Network settings initialized"));
}

void Afps_cppGameMode::StartGame()
{
	
}
