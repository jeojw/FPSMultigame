// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "fps_cppGameMode.h"
#include "StartMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API UStartMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SignupButton;

	UPROPERTY(meta = (BindWidget))
	UButton* OptionButton;

	UPROPERTY()
	Afps_cppGameMode* GameMode;

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void GoToSignup();

	UFUNCTION()
	void GoToLogin();

	UFUNCTION()
	void GoToOption();
};
