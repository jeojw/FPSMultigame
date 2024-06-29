// Fill out your copyright notice in the Description page of Project Settings.


#include "fps_cppPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

Afps_cppPlayerController::Afps_cppPlayerController() : APlayerController()
{
    PrimaryActorTick.bCanEverTick = true;

    Database = CreateDefaultSubobject<UMyDatabaseManager>(TEXT("DatabaseManager"));

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
}

void Afps_cppPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalPlayerController())
    {
        ClientInitializeUI();
    }

    InitializeDatabase();
}

void Afps_cppPlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    if (IsLocalPlayerController())
    {
        ClientInitializeUI();
    }

    InitializeDatabase();
}

void Afps_cppPlayerController::InitializeDatabase()
{
    if (Database)
    {
        FString DatabasePath = TEXT("Database/FPSGame"); // 데이터베이스 파일 경로
        FString SQLFilePath = TEXT("Database/FPSGameDatabase.sql"); // SQL 파일 경로
        if (Database->OpenDatabase(DatabasePath))
        {
            UE_LOG(LogTemp, Log, TEXT("Database opened successfully."));

            if (Database->ExecuteSQLFile(SQLFilePath))
            {
                UE_LOG(LogTemp, Log, TEXT("SQL file executed successfully."));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to execute SQL file."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to open database."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DatabaseManager is null."));
    }
}

void Afps_cppPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void Afps_cppPlayerController::ClientInitializeUI_Implementation()
{
    InitializeUI();

    SetPause(true);
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    FInputModeUIOnly InputMode;
    SetInputMode(InputMode);
}

void Afps_cppPlayerController::InitializeUI()
{
    if (!PlayerUIWidgetInstance && PlayerUIWidgetClass)
    {
        PlayerUIWidgetInstance = CreateWidget<UUserWidget>(this, PlayerUIWidgetClass);
        if (PlayerUIWidgetInstance)
        {
            PlayerUIWidgetInstance->AddToViewport();
            PlayerUIWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    if (!StartWidgetInstance && StartWidgetClass)
    {
        StartWidgetInstance = CreateWidget<UUserWidget>(this, StartWidgetClass);
        if (StartWidgetInstance)
        {
            StartWidgetInstance->AddToViewport();
            StartWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        }
    }

    if (!LoginWidgetInstance && LoginWidgetClass)
    {
        LoginWidgetInstance = CreateWidget<UUserWidget>(this, LoginWidgetClass);
        if (LoginWidgetInstance)
        {
            LoginWidgetInstance->AddToViewport();
            LoginWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    if (!SignupWidgetInstance && SignupWidgetClass)
    {
        SignupWidgetInstance = CreateWidget<UUserWidget>(this, SignupWidgetClass);
        if (SignupWidgetInstance)
        {
            SignupWidgetInstance->AddToViewport();
            SignupWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    if (!OptionWidgetInstance && OptionWidgetClass)
    {
        OptionWidgetInstance = CreateWidget<UUserWidget>(this, OptionWidgetClass);
        if (OptionWidgetInstance)
        {
            OptionWidgetInstance->AddToViewport();
            OptionWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void Afps_cppPlayerController::VisibleStartMenu()
{
    if (SignupWidgetInstance && StartWidgetInstance)
    {
        StartWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        SignupWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
    }
}

void Afps_cppPlayerController::VisibleSignup()
{
    if (SignupWidgetInstance && StartWidgetInstance)
    {
        SignupWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        StartWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
    }
}

void Afps_cppPlayerController::VisibleLogin()
{
    if (LoginWidgetInstance && StartWidgetInstance && SignupWidgetInstance)
    {
        LoginWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        StartWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        if (SignupWidgetInstance->IsVisible())
        {
            SignupWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void Afps_cppPlayerController::VisibleOption()
{
    if (OptionWidgetInstance && StartWidgetInstance)
    {
        OptionWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        StartWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
    }
}

void Afps_cppPlayerController::VisiblePlayerUI()
{
    if (LoginWidgetInstance && PlayerUIWidgetInstance)
    {
        PlayerUIWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        LoginWidgetInstance->SetVisibility(ESlateVisibility::Hidden);

        SetPause(false);
        bShowMouseCursor = false;
        bEnableClickEvents = false;
        bEnableMouseOverEvents = false;

        FInputModeGameOnly InputMode;
        SetInputMode(InputMode);
    }
}

bool Afps_cppPlayerController::CheckIdDuplicate(const FString& MemberID)
{
    if (Database)
    {
        return Database->CheckDuplicateID(MemberID);
    }
    return false;
}
bool Afps_cppPlayerController::CheckNicknameDuplicate(const FString& MemberNickname)
{
    if (Database)
    {
        return Database->CheckDuplicateNickname(MemberNickname);
    }
    return false;
}
bool Afps_cppPlayerController::SignupPlayer(const FString& MemberID, const FString& MemberPW, const FString& MemberNickname)
{
    if (Database)
    {
        return Database->InsertPlayerData(MemberID, MemberPW, MemberNickname);
    }
    return false;
}

bool Afps_cppPlayerController::LoginPlayer(const FString& MemberID, const FString& MemberPW)
{
    if (Database)
    {
        return Database->CheckPlayerData(MemberID, MemberPW);
    }
    return false;
}

bool Afps_cppPlayerController::CheckMultipleLogin(const FString& MemberID)
{
    if (Database)
    {
        return Database->LogInPlayer(MemberID);
    }
    return false;
}

void Afps_cppPlayerController::SetPlayerID(const FString& _PlayerID)
{
    this->PlayerID = _PlayerID;
}

FString Afps_cppPlayerController::GetPlayerID()
{
    return this->PlayerID;
}
