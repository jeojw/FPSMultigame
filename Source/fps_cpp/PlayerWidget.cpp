// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"
#include "fps_cppCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "fps_cppGameMode.h"
#include "CanvasItem.h"

void UPlayerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // UI 요소 초기화
    if (HpBar)
    {
        HpBar->SetPercent(1.0f);  // HP 바를 100%로 초기화
    }
    if (CurBulletCounts)
    {
        CurBulletCounts->SetText(FText::FromString("0"));  // 초기 총알 개수 설정
    }
    if (DeathMessage)
    {
        DeathMessage->SetVisibility(ESlateVisibility::Hidden);
    }
    if (RespawnBar)
    {
        RespawnBar->SetVisibility(ESlateVisibility::Hidden);
    }

    // 딜레이를 주고 플레이어 캐릭터를 가져옴
    UpdatePlayerState();
}

void UPlayerWidget::UpdatePlayerState()
{
    APlayerController* PlayerController = GetOwningPlayer();
    if (PlayerController)
    {
        Afps_cppPlayerController* FPSController = Cast<Afps_cppPlayerController>(PlayerController);
        PlayerState = Cast<Afps_cppPlayerState>(FPSController->PlayerState);
    }

    if (PlayerState)
    {
        PlayerMaxHealth = PlayerState->GetMaxHealth();
        PlayerHealth = PlayerState->GetHealth();
        CurItem = PlayerState->GetCurrentWeaponType();
        CurItemSelection = PlayerState->GetCurrentItemSelection();
        CurPistols = PlayerState->GetInventory()->GetCurBullet(CurItemSelection);
        CurWeaponIcon = LoadObject<UPaperSprite>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/Weapons/Weapon_Icons/Weapon_Icon_WeaponIcon_12"));

        if (CurBulletCounts)
        {
            CurBulletCounts->SetText(FText::FromString(FString::FromInt(CurPistols)));  // 초기 총알 개수 설정
        }
    }
    else
    {
        // Player가 Null인 경우 경고 로그 출력 및 재시도
        UE_LOG(LogTemp, Warning, TEXT("Player is null in UpdatePlayerHealth, retrying..."));
        PlayerHealth = 0.0f;  // 예시 값으로 초기화
        if (CurBulletCounts)
        {
            CurBulletCounts->SetText(FText::FromString("0"));
        }

        // 재시도: 타이머를 사용하여 다시 시도
        FTimerHandle RetryHandle;
        GetWorld()->GetTimerManager().SetTimer(RetryHandle, this, &UPlayerWidget::UpdatePlayerState, 1.0f, false);
    }
}

void UPlayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (PlayerState)
    {
        CurPistols = PlayerState->GetInventory()->GetCurBullet(CurItemSelection); // 플레이어의 총알 개수 업데이트 (예시)
        PlayerHealth = PlayerState->GetHealth(); // 플레이어의 체력 업데이트 (예시)
        CurWeaponIcon = PlayerState->GetWeaponIcon();

        if (CurBulletCounts)
        {
            CurBulletCounts->SetText(FText::FromString(FString::FromInt(CurPistols)));
        }

        if (HpBar)
        {
            HpBar->SetPercent(FMath::Clamp(PlayerHealth / PlayerMaxHealth, 0.0f, 1.0f));
        }

        if (PlayerState->GetIsDead())
        {
            DeathMessage->SetVisibility(ESlateVisibility::Visible);
            RespawnBar->SetVisibility(ESlateVisibility::Visible);
        }
        else 
        {
            DeathMessage->SetVisibility(ESlateVisibility::Hidden);
            RespawnBar->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

UTexture2D* UPlayerWidget::ConvertSpriteToTexture(UPaperSprite* Sprite)
{
    if (!Sprite)
    {
        UE_LOG(LogTemp, Error, TEXT("ConvertSpriteToTexture: Sprite is NULL"));
        return nullptr;
    }

    UTexture2D* SourceTexture = Sprite->GetBakedTexture();
    if (!SourceTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("ConvertSpriteToTexture: SourceTexture is NULL"));
        return nullptr;
    }

    UE_LOG(LogTemp, Log, TEXT("ConvertSpriteToTexture: SourceTexture is valid"));

    // SourceTexture의 크기와 포맷을 사용하여 새로운 텍스처 생성
    UTexture2D* NewTexture = UTexture2D::CreateTransient(SourceTexture->GetSizeX(), SourceTexture->GetSizeY(), SourceTexture->GetPixelFormat());
    if (!NewTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("ConvertSpriteToTexture: Failed to create NewTexture"));
        return nullptr;
    }

    // 유효성 검사 추가
    if (!NewTexture->GetPlatformData() || !SourceTexture->GetPlatformData())
    {
        UE_LOG(LogTemp, Error, TEXT("ConvertSpriteToTexture: PlatformData is NULL"));
        return nullptr;
    }

    if (NewTexture->GetPlatformData()->Mips.Num() == 0 || SourceTexture->GetPlatformData()->Mips.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("ConvertSpriteToTexture: Mip data is missing"));
        return nullptr;
    }

    void* TextureData = nullptr;
    const void* SourceData = nullptr;

    // 잠금 및 해제 과정에서 예외 처리를 추가하여 안전하게 관리
    try
    {
        SourceData = SourceTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_ONLY);
        TextureData = NewTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);

        if (!TextureData || !SourceData)
        {
            UE_LOG(LogTemp, Error, TEXT("ConvertSpriteToTexture: Failed to lock Mip data"));
            if (TextureData)
            {
                NewTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
            }
            if (SourceData)
            {
                SourceTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
            }
            return nullptr;
        }

        FMemory::Memcpy(TextureData, SourceData, SourceTexture->GetPlatformData()->Mips[0].BulkData.GetBulkDataSize());

        NewTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
        SourceTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

        NewTexture->UpdateResource();
    }
    catch (...)
    {
        if (TextureData)
        {
            NewTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
        }
        if (SourceData)
        {
            SourceTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
        }
        UE_LOG(LogTemp, Error, TEXT("ConvertSpriteToTexture: Exception occurred while locking/unlocking Mip data"));
        return nullptr;
    }

    return NewTexture;
}

void UPlayerWidget::SetImageBrushFromTexture(UImage* ImageWidget, UTexture2D* Texture)
{
    if (ImageWidget && Texture)
    {
        FSlateBrush Brush;
        Brush.SetResourceObject(Texture);
        Brush.ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
        ImageWidget->SetBrush(Brush);
    }
    else
    {
        if (!ImageWidget)
        {
            UE_LOG(LogTemp, Error, TEXT("SetImageBrushFromTexture: ImageWidget is NULL"));
        }
        if (!Texture)
        {
            UE_LOG(LogTemp, Error, TEXT("SetImageBrushFromTexture: Texture is NULL"));
        }
    }
}

void UPlayerWidget::SetWeaponImage(UPaperSprite* NewSprite)
{
    if (CurWeaponImage)
    {
        UTexture2D* Texture = ConvertSpriteToTexture(NewSprite);
        if (Texture)
        {
            SetImageBrushFromTexture(CurWeaponImage, Texture);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("SetWeaponImage: Failed to convert sprite to texture"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SetWeaponImage: CurWeaponImage is NULL"));
    }
}