// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "Kismet/GameplayStatics.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CheckLoginButton)
	{
		CheckLoginButton->OnClicked.AddDynamic(this, &ULoginWidget::CheckLogin);
	}
	
	PlayerController = Cast<Afps_cppPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void ULoginWidget::CheckLogin()
{
	PlayerController->VisiblePlayerUI();
}