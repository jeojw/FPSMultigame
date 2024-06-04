// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "fps_cppCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

	UCanvasPanel* UICanvas;
	UProgressBar* HpBar;
	UImage* AimCross;
	UImage* curWeaponImage;
	UImage* curPistolImage;
	FText* curPistolCounts;

	int PlayerHealth;
	EItemTypeEnum CurItem;
	int curPistols;

public:
};
