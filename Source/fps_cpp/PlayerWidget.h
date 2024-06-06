// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "fps_cppCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* UICanvas;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HpBar;

	UPROPERTY(meta = (BindWidget))
	UImage* AimCross;

	UPROPERTY(meta = (BindWidget))
	UImage* CurWeaponImage;

	UPROPERTY(meta = (BindWidget))
	UImage* CurPistolImage_1;
	UPROPERTY(meta = (BindWidget))
	UImage* CurPistolImage_2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurPistolCounts;

	UPROPERTY()
	Afps_cppCharacter* Player;

	int CurItemSelection;
	int PlayerHealth;
	EItemTypeEnum CurItem;
	int CurPistols;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
