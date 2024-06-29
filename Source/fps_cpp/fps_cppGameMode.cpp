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
	
	RespawnTime = 3.0f;
}

void Afps_cppGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitializeDatabase();

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
				Respawn(MyPlayerController, PlayerState);
			}
		}
	}
}

void Afps_cppGameMode::InitializeDatabase()
{
	DatabaseManager = NewObject<UMyDatabaseManager>();

	FString DatabasePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("database.sqlite3"));

	if (DatabaseManager->OpenDatabase(DatabasePath))
	{
		UE_LOG(LogTemp, Log, TEXT("Database opened successfully."));
		DatabaseManager->CreateTable();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to open database."));
	}
}

void Afps_cppGameMode::LoadPlayerData(const FString& MemberID)
{
	if (DatabaseManager)
	{
		FString MemberPW;
		FString MemberNickname;

		if (DatabaseManager->GetPlayerData(MemberID, MemberPW, MemberNickname))
		{
			UE_LOG(LogTemp, Log, TEXT("Player data retrieved: %s, %s"), *MemberPW, *MemberNickname);
			// 게임 내에서 플레이어 데이터를 사용하는 로직 추가
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to retrieve player data."));
		}
	}
}

void Afps_cppGameMode::SavePlayerData(const FString& MemberID, const FString& MemberPW, const FString& MemberNickname)
{
	if (DatabaseManager)
	{
		if (DatabaseManager->InsertPlayerData(MemberID, MemberPW, MemberNickname))
		{
			UE_LOG(LogTemp, Log, TEXT("Player data inserted successfully."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to insert player data."));
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
}

void Afps_cppGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	Afps_cppPlayerController* MyPlayerController = Cast<Afps_cppPlayerController>(Exiting);
	if (MyPlayerController)
	{
		MyPlayerController->LogoutPlayer(MyPlayerController->GetPlayerID());
		MyPlayerController->SetPlayerID("");
	}
}

void Afps_cppGameMode::Respawn(Afps_cppPlayerController* PlayerController, Afps_cppPlayerState* PlayerState)
{
	if (GetWorld() && !PlayerRespawnRequested.Contains(PlayerState))
	{
		FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &Afps_cppGameMode::RespawnFunction, PlayerController, PlayerState);

		FTimerHandle& RespawnTimerHandle = PlayerRespawnTimers.FindOrAdd(PlayerState);
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnTime, false);

		// 부활 요청된 플레이어 상태를 추가
		PlayerRespawnRequested.Add(PlayerState);
	}
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

			APawn* OldPawn = PlayerController->GetPawn();
			if (OldPawn)
			{
				OldPawn->Destroy();
			}

			Afps_cppCharacter* NewPawn = World->SpawnActor<Afps_cppCharacter>(DefaultPawnClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (NewPawn)
			{
				PlayerController->Possess(NewPawn);

				if (HasAuthority())
				{
					NewPawn->ActivateObjectMulticast();
				}
				else
				{
					NewPawn->ActivateObjectServer();
				}

				// 부활 후 플래그 및 타이머 제거
				PlayerRespawnRequested.Remove(PlayerState);
				PlayerRespawnTimers.Remove(PlayerState);
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
