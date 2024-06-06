// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"
#include "fps_cppCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "CanvasItem.h"

void UPlayerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    Player = Cast<Afps_cppCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    // 위젯 변수 초기화
    PlayerHealth = Player->GetHealth();  // 예시 값
    CurItem = Player->GetWeaponType();  // EItemTypeEnum의 초기값 설정
    CurItemSelection = Player->GetCurrentItemSelection();
    CurPistols = Player->GetInventory()->GetCurBullet(CurItemSelection);
    CurWeaponIcon = LoadObject<UPaperSprite>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/Weapons/Weapon_Icons/Weapon_Icon_WeaponIcon_12"));

    if (HpBar)
    {
        HpBar->SetPercent(1.0f);  // HP 바를 100%로 초기화
    }

    if (CurBulletCounts)
    {
        CurBulletCounts->SetText(FText::FromString(FString::FromInt(CurPistols)));  // 초기 총알 개수 설정
    }
}

void UPlayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (Player)
    {
        CurPistols = Player->GetInventory()->GetCurBullet(CurItemSelection); // 플레이어의 총알 개수 업데이트 (예시)
        PlayerHealth = Player->GetHealth(); // 플레이어의 체력 업데이트 (예시)
        CurWeaponIcon = Player->GetWeaponIcon();

        if (CurBulletCounts)
        {
            CurBulletCounts->SetText(FText::FromString(FString::FromInt(CurPistols)));
        }

        if (HpBar)
        {
            HpBar->SetPercent(FMath::Clamp(PlayerHealth / 100.0f, 0.0f, 1.0f));
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