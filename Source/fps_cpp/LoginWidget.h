// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* UICanvas;

	UPROPERTY(meta = (BindWidget))
	UImage* BackgroundImage;

	UPROPERTY(meta = (BindWidget))
	UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LoginButtonText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Signin;

	UPROPERTY(meta = (BindWidget))
	UEditableText* InputID;

	UPROPERTY(meta = (BindWidget))
	UEditableText* InputPassword;
	
};
