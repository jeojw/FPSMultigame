// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "fps_cppPlayerController.h"
#include "SignupWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API USignupWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UEditableText* InputNewId;

	UPROPERTY(meta = (BindWidget))
	UEditableText* InputNewPw;

	UPROPERTY(meta = (BindWidget))
	UEditableText* InputNewNickname;

	UPROPERTY(meta = (BindWidget))
	UButton* CheckId;

	UPROPERTY(meta = (BindWidget))
	UButton* CheckNickname;

	UPROPERTY(meta = (BindWidget))
	UButton* SignupButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CheckIdMessage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CheckNicknameMessage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CheckPwMessage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CheckInfoMessage;

	UPROPERTY()
	Afps_cppPlayerController* PlayerController;

	bool isCheckId;
	bool isCheckNickname;
	bool isCheckAll;

	FString IdString;
	FString PasswordString;
	FString NicknameString;

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void OnIdTextChanged(const FText& Text);

	UFUNCTION()
	void OnPasswordTextChanged(const FText& Text);

	UFUNCTION()
	void OnNicknameTextChanged(const FText& Text);

	UFUNCTION()
	void CheckIdDuplicate();

	UFUNCTION()
	void CheckNicknameDuplicate();

	UFUNCTION()
	void Signup();

	UFUNCTION()
	void CancelSignup();

	UFUNCTION()
	void UpdateValidationMessages();
};
