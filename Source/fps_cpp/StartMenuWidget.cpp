// Fill out your copyright notice in the Description page of Project Settings.


#include "StartMenuWidget.h"
#include "Kismet/GameplayStatics.h"

void UStartMenuWidget::NativeConstruct()
{
	if (LoginButton)
	{
		LoginButton->OnClicked.AddDynamic(this, &UStartMenuWidget::GoToLogin);
	}

	if (SignupButton)
	{
		SignupButton->OnClicked.AddDynamic(this, &UStartMenuWidget::GoToSignup);
	}

	if (OptionButton)
	{
		OptionButton->OnClicked.AddDynamic(this, &UStartMenuWidget::GoToOption);
	}

	PlayerController = Cast<Afps_cppPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void UStartMenuWidget::GoToSignup()
{
	PlayerController->VisibleSignup();
}

void UStartMenuWidget::GoToLogin()
{
	PlayerController->VisibleLogin();
}

void UStartMenuWidget::GoToOption()
{
	PlayerController->VisibleOption();
}