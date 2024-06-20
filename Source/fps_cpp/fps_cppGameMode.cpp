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

	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerUIFinder(TEXT("/Game/ThirdPerson/Blueprints/Widget/PlayerMainUI"));
	if (PlayerUIFinder.Succeeded())
	{
		PlayerUIWidgetClass = PlayerUIFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> StartWidgetFinder(TEXT("/Game/ThirdPerson/Blueprints/Widget/BP_StartMenuWidget"));
	if (StartWidgetFinder.Succeeded())
	{
		StartWidgetClass = StartWidgetFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> LoginWidgetFinder(TEXT("/Game/ThirdPerson/Blueprints/Widget/BP_LoginWidget"));
	if (LoginWidgetFinder.Succeeded())
	{
		LoginWidgetClass = LoginWidgetFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> SignupWidgetFinder(TEXT("/Game/ThirdPerson/Blueprints/Widget/BP_SignupWidget"));
	if (SignupWidgetFinder.Succeeded())
	{
		SignupWidgetClass = SignupWidgetFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> OptionWidgetFinder(TEXT("/Game/ThirdPerson/Blueprints/Widget/BP_OptionWidget"));
	if (OptionWidgetFinder.Succeeded())
	{
		OptionWidgetClass = OptionWidgetFinder.Class;
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

	if (PlayerUIWidgetClass)
	{
		PlayerUIWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), PlayerUIWidgetClass);
		if (PlayerUIWidgetInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("UI is called"));
			PlayerUIWidgetInstance->AddToViewport();
			PlayerUIWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (StartWidgetClass)
	{
		StartWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), StartWidgetClass);
		if (StartWidgetInstance)
		{
			StartWidgetInstance->AddToViewport();
			StartWidgetInstance->SetVisibility(ESlateVisibility::Visible);
			PauseGame();
		}
	}

	if (LoginWidgetClass)
	{
		LoginWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), LoginWidgetClass);
		if (LoginWidgetInstance)
		{
			LoginWidgetInstance->AddToViewport();
			LoginWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (SignupWidgetClass)
	{
		SignupWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), SignupWidgetClass);
		if (SignupWidgetInstance)
		{
			SignupWidgetInstance->AddToViewport();
			SignupWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (OptionWidgetClass)
	{
		OptionWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), OptionWidgetClass);
		if (OptionWidgetInstance)
		{
			OptionWidgetInstance->AddToViewport();
			OptionWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
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

void Afps_cppGameMode::PauseGame()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		PlayerController->SetPause(true);
		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;
	}
}

void Afps_cppGameMode::UnpauseGame()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		PlayerController->SetPause(false);
		PlayerController->bShowMouseCursor = false;
		PlayerController->bEnableClickEvents = false;
		PlayerController->bEnableMouseOverEvents = false;

		if (StartBGMAudioComponent)
		{
			StartBGMAudioComponent->Stop();
		}
	}
}

void Afps_cppGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

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

void Afps_cppGameMode::VisibleSignup()
{
	if (SignupWidgetInstance && StartWidgetInstance)
	{
		SignupWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		StartWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}
}

void Afps_cppGameMode::VisibleLogin()
{
	if (LoginWidgetInstance && StartWidgetInstance)
	{
		LoginWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		StartWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}
}

void Afps_cppGameMode::VisibleOption()
{
	if (OptionWidgetInstance && StartWidgetInstance)
	{
		OptionWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		StartWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}
}

void Afps_cppGameMode::VisiblePlayerUI()
{
	if (LoginWidgetInstance && PlayerUIWidgetInstance)
	{
		PlayerUIWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		LoginWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		UnpauseGame();
	}
}
