// Fill out your copyright notice in the Description page of Project Settings.


#include "fps_cppPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Engine/Texture2D.h"
#include "Misc/FileHelper.h"

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

    static ConstructorHelpers::FClassFinder<UUserWidget> LobbyWidgetFinder(TEXT("/Game/ThirdPerson/Blueprints/Widget/BP_LobbyWidget"));
    if (LobbyWidgetFinder.Succeeded())
    {
        LobbyWidgetClass = LobbyWidgetFinder.Class;
    }
}

void Afps_cppPlayerController::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    InitializePlayerState();
}

void Afps_cppPlayerController::BeginPlay()
{
    Super::BeginPlay();

    InitializePlayerState();
}

void Afps_cppPlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    InitializePlayerState();
}

void Afps_cppPlayerController::InitializePlayerState()
{
    FPSPlayerState = GetPlayerState<Afps_cppPlayerState>();

    if (FPSPlayerState)
    {
        if (IsLocalController())
        {
            ServerInitializeUI();
        }

        InitializeDatabase();
    }
    else
    {
        // PlayerState가 아직 null인 경우, 일정 시간 후에 다시 시도
        UE_LOG(LogTemp, Warning, TEXT("PlayerState is null in InitializePlayerState. Retrying in 0.1 seconds..."));
        GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
            {
                InitializePlayerState();
            });
    }
}

void Afps_cppPlayerController::InitializeDatabase()
{
    if (Database)
    {
        FString DatabasePath = TEXT("Database/FPSGame"); // �����ͺ��̽� ���� ���
        FString SQLFilePath = TEXT("Database/FPSGameDatabase.sql"); // SQL ���� ���
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

    if (!FPSPlayerState)
    {
        FPSPlayerState = GetPlayerState<Afps_cppPlayerState>();
    }
}

void Afps_cppPlayerController::ClientInitializeUI_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("ClientInitializeUI called"));

    InitializeUI();

    SetPause(true);
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    FInputModeUIOnly InputMode;
    SetInputMode(InputMode);
}

void Afps_cppPlayerController::ServerInitializeUI_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("ClientInitializeUI called"));

    InitializeUI();

    SetPause(true);
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    FInputModeUIOnly InputMode;
    SetInputMode(InputMode);

    ClientInitializeUI();
}

bool Afps_cppPlayerController::ServerInitializeUI_Validate()
{
    return true;
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

    if (!LobbyWidgetInstance && LobbyWidgetClass)
    {
        LobbyWidgetInstance = CreateWidget<UUserWidget>(this, LobbyWidgetClass);
        if (LobbyWidgetInstance)
        {
            LobbyWidgetInstance->AddToViewport();
            LobbyWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void Afps_cppPlayerController::VisibleStartMenu()
{
    if (SignupWidgetInstance && StartWidgetInstance && LobbyWidgetInstance)
    {
        StartWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        SignupWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        if (LobbyWidgetInstance->IsVisible())
        {
            LobbyWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
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

void Afps_cppPlayerController::VisibleLobbyUI()
{
    if (LoginWidgetInstance && LobbyWidgetInstance)
    {
        LobbyWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        LoginWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
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

UTexture2D* Afps_cppPlayerController::LoadTextureFromFile(const FString& FilePath)
{
    TArray<uint8> RawFileData;
    if (!FFileHelper::LoadFileToArray(RawFileData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
        return nullptr;
    }

    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(RawFileData.GetData(), RawFileData.Num());
    if (ImageFormat == EImageFormat::Invalid)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid image format: %s"), *FilePath);
        return nullptr;
    }

    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
    if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse image: %s"), *FilePath);
        return nullptr;
    }

    // UncompressedRGBA를 TArray64<int8>로 선언하여 제대로 된 데이터 형식을 사용
    TArray<uint8> UncompressedRGBA;
    if (!ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get raw image data: %s"), *FilePath);
        return nullptr;
    }

    UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
    if (!Texture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create texture: %s"), *FilePath);
        return nullptr;
    }

    // Texture 데이터 설정
    void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
    FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
    Texture->GetPlatformData()->Mips[0].BulkData.Unlock();

    Texture->UpdateResource();

    return Texture;
}

bool Afps_cppPlayerController::LoginPlayer(const FString& MemberID, const FString& MemberPW)
{
    if (!FPSPlayerState)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerState is not valid in LoginPlayer"));
        return false;
    }

    if (Database)
    {
        if (Database->CheckPlayerData(MemberID, MemberPW))
        {
            IsLogin = true;

            FString nickname, ProfileImagePath;
            Database->GetPlayerData(MemberID, nickname, ProfileImagePath);
            UTexture2D* ProfileImage = LoadTextureFromFile(ProfileImagePath);
            if (HasAuthority())
            {
                FPSPlayerState->SetPlayerInfoMulticast(MemberID, nickname, ProfileImage);
            }
            else
            {
                FPSPlayerState->SetPlayerInfoServer(MemberID, nickname, ProfileImage);
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

bool Afps_cppPlayerController::LogoutPlayer()
{
    if (!FPSPlayerState)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerState is not valid in LoginPlayer"));
        return false;
    }

    if (Database)
    {
        IsLogOut = Database->LogOutPlayer(FPSPlayerState->GetPlayer_Id());
        if (HasAuthority())
        {
            FPSPlayerState->SetPlayerInfoMulticast("", "", nullptr);
        }
        else
        {
            FPSPlayerState->SetPlayerInfoServer("", "", nullptr);
        }
        return IsLogOut;
    }
    return false;
}

bool Afps_cppPlayerController::CheckMultipleLogin(const FString& MemberID)
{
    if (Database)
    {
        IsLogin = Database->LogInPlayer(MemberID);
        return IsLogin;
    }
    return false;
}

void Afps_cppPlayerController::InitializePlayerAfterLogin_Implementation(TSubclassOf<APawn> PawnClass)
{
    ClientInitializeUI();

    FVector InitSpawnLocation = GetPawn()->GetActorLocation();
    FRotator InitSpawnRotation = GetPawn()->GetActorRotation();

    AActor* SpawnedActor = GetWorld()->SpawnActor(PawnClass, &InitSpawnLocation, &InitSpawnRotation);
    if (SpawnedActor)
    {
        Possess(Cast<APawn>(SpawnedActor));
    }
}

bool Afps_cppPlayerController::InitializePlayerAfterLogin_Validate(TSubclassOf<APawn> PawnClass)
{
    return true;
}

void Afps_cppPlayerController::ClientShowLoginFailedMessage_Implementation()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Login failed. Please check your ID and Password."));
}
