// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"
#include "fps_cppCharacter.h"
#include "Kismet/GameplayStatics.h"

void UPlayerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    Player = Cast<Afps_cppCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    // 위젯 변수 초기화
    PlayerHealth = Player->GetHealth();  // 예시 값
    CurItem = Player->GetWeaponType();  // EItemTypeEnum의 초기값 설정
    CurItemSelection = Player->GetCurrentItemSelection();
    CurPistols = Player->GetInventory()->GetCurBullet(CurItemSelection);

    if (HpBar)
    {
        HpBar->SetPercent(1.0f);  // HP 바를 100%로 초기화
    }

    if (CurPistolCounts)
    {
        CurPistolCounts->SetText(FText::FromString(FString::FromInt(CurPistols)));  // 초기 총알 개수 설정
    }
}

void UPlayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (Player)
    {
        CurPistols = Player->GetInventory()->GetCurBullet(CurItemSelection); // 플레이어의 총알 개수 업데이트 (예시)
        PlayerHealth = Player->GetHealth(); // 플레이어의 체력 업데이트 (예시)

        if (CurPistolCounts)
        {
            CurPistolCounts->SetText(FText::FromString(FString::FromInt(CurPistols)));
        }

        if (HpBar)
        {
            HpBar->SetPercent(FMath::Clamp(PlayerHealth / 100.0f, 0.0f, 1.0f));
        }
    }
}