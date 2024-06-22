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
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<Afps_cppCharacter> PlayerPawnBPClassFinder(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClassFinder.Succeeded())
	{
		PlayerPawnBPClass = PlayerPawnBPClassFinder.Class;
		DefaultPawnClass = PlayerPawnBPClass;
	}

	static ConstructorHelpers::FObjectFinder<Afps_cppCharacter> PlayerPawnBPObjectFinder(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPObjectFinder.Succeeded())
	{
		Player = PlayerPawnBPObjectFinder.Object;
	}

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
	if (Player && Player->GetIsDead())
	{
		UE_LOG(LogTemp, Log, TEXT("Respawn!!"));
		Respawn();
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
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &Afps_cppGameMode::RespawnFunction, RespawnTime, false);
	}
}

void Afps_cppGameMode::RespawnFunction()
{
    if (PlayerPawnBPClass != nullptr)
    {
		if (GetWorld())
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				Afps_cppCharacter* NewPawn = GetWorld()->SpawnActor<Afps_cppCharacter>(PlayerPawnBPClass, FVector(900.0f, 1100.0f, 92.0f), FRotator::ZeroRotator);
				if (NewPawn)
				{
					PlayerController->Possess(NewPawn);

					Player = NewPawn;
					if (Player)
					{
						Player->SetIsDead(false);
					}
				}

			}
		}
    }

    GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
}

void Afps_cppGameMode::InitializeNetworkSettings()
{
	// 네트워크 설정 초기화 (필요한 경우 추가 설정)
	UE_LOG(LogTemp, Warning, TEXT("Network settings initialized"));
}

void Afps_cppGameMode::StartGame()
{
	
}
