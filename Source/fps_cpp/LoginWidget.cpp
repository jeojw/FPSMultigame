// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "Kismet/GameplayStatics.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (InputID)
	{
		InputID->OnTextChanged.AddDynamic(this, &ULoginWidget::OnIdTextChanged);
	}

	if (InputPassword)
	{
		InputPassword->SetKeyboardType(EVirtualKeyboardType::Password);
		InputPassword->SetIsPassword(true);
		InputPassword->OnTextChanged.AddDynamic(this, &ULoginWidget::OnPasswordTextChanged);
	}

	if (CheckLoginButton)
	{
		CheckLoginButton->OnClicked.AddDynamic(this, &ULoginWidget::CheckLogin);
	}
	
	PlayerController = Cast<Afps_cppPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void ULoginWidget::OnIdTextChanged(const FText& Text)
{
	IdString = Text.ToString();
}

void ULoginWidget::OnPasswordTextChanged(const FText& Text)
{
	PasswordString = Text.ToString();
}

void ULoginWidget::CheckLogin()
{
	if (InputID && InputPassword)
	{
		if (InputID->GetText().IsEmpty() || InputID->GetText().IsEmpty())
		{
			return;
		}
		if (PlayerController->LoginPlayer(InputID->GetText().ToString(), InputPassword->GetText().ToString()))
		{
			if (PlayerController->CheckMultipleLogin(InputID->GetText().ToString()))
			{
				PlayerController->SetPlayerID(InputID->GetText().ToString());
				PlayerController->VisiblePlayerUI();
			}
		}
	}
}