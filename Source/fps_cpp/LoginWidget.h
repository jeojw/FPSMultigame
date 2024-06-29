// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "fps_cppPlayerController.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UButton* CheckLoginButton;

	UPROPERTY(meta = (BindWidget))
	UEditableText* InputID;

	UPROPERTY(meta = (BindWidget))
	UEditableText* InputPassword;
	
	UPROPERTY()
	Afps_cppPlayerController* PlayerController;
	
	FString IdString;
	FString PasswordString;

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void OnIdTextChanged(const FText& Text);

	UFUNCTION()
	void OnPasswordTextChanged(const FText& Text);

	UFUNCTION()
	void CheckLogin();
};
