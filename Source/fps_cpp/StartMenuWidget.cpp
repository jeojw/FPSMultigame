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

	GameMode = Cast<Afps_cppGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}

void UStartMenuWidget::GoToSignup()
{
	GameMode->VisibleSignup();
}

void UStartMenuWidget::GoToLogin()
{
	GameMode->VisibleLogin();
}

void UStartMenuWidget::GoToOption()
{
	GameMode->VisibleOption();
}