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

	static ConstructorHelpers::FClassFinder<UUserWidget> StartWidgetFinder(TEXT("/Game/ThirdPerson/Blueprints/Widget/BP_StartMenuWidget"));
	if (StartWidgetFinder.Succeeded())
	{
		StartWidgetClass = StartWidgetFinder.Class;
	}
	
	RespawnTime = 1.0f;
}

void Afps_cppGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (StartWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), StartWidgetClass);
		if (StartWidgetClass != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->Possess(Player);
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
