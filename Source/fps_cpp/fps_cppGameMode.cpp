// Copyright Epic Games, Inc. All Rights Reserved.

#include "fps_cppGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

Afps_cppGameMode::Afps_cppGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClassFinder(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClassFinder.Class != NULL)
	{
		PlayerPawnBPClass = PlayerPawnBPClassFinder.Class;
		DefaultPawnClass = PlayerPawnBPClass;
	}

	RespawnTime = 10.0f;
}

void Afps_cppGameMode::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<Afps_cppCharacter>(PlayerPawnBPClass);
	InitializeNetworkSettings();
}

void Afps_cppGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (Player && Player->GetIsDead())
	{
		Respawn();
	}
}

void Afps_cppGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
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
				APawn* OldPawn = PlayerController->GetPawn();
				APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PlayerPawnBPClass, FVector(900.0f, 1100.0f, 92.0f), FRotator::ZeroRotator);
				if (NewPawn)
				{
					if (OldPawn)
					{
						OldPawn->Destroy();
					}

					PlayerController->Possess(NewPawn);

					Player = Cast<Afps_cppCharacter>(NewPawn);
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
