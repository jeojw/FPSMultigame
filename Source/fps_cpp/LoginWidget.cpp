// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "Kismet/GameplayStatics.h"

void ULoginWidget::NativeConstruct()
{
	if (CheckLoginButton)
	{
		CheckLoginButton->OnClicked.AddDynamic(this, &ULoginWidget::CheckLogin);
	}
	
	GameMode = Cast<Afps_cppGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}

void ULoginWidget::CheckLogin()
{
	GameMode->VisiblePlayerUI();
}