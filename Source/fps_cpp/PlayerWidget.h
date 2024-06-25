// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "fps_cppPlayerState.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "ImageUtils.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "Modules/ModuleManager.h"
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
	UImage* CurBulletImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurBulletCounts;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathMessage;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* RespawnBar;

	UPROPERTY()
	Afps_cppPlayerState* PlayerState;

	UPROPERTY()
	UPaperSprite* CurWeaponIcon;

	int RespawnProgress;
	int CurItemSelection;
	float PlayerMaxHealth;
	float PlayerHealth;
	EItemTypeEnum CurItem;
	int CurPistols;

	void FetchPlayerCharacter();
	void UpdatePlayerState();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void SetWeaponImage(UPaperSprite* NewSprite);
	UTexture2D* ConvertSpriteToTexture(UPaperSprite* Sprite);
	void SetImageBrushFromTexture(UImage* ImageWidget, UTexture2D* Texture);
};
