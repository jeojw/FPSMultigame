// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

void ULobbyWidget::NativeConstruct()
{
	RoomTitle_1->SetVisibility(ESlateVisibility::Hidden);
	curPlayers_1->SetVisibility(ESlateVisibility::Hidden);
	Divide_1->SetVisibility(ESlateVisibility::Hidden);
	maxPlayers_1->SetVisibility(ESlateVisibility::Hidden);
	EntranceButton_1->SetVisibility(ESlateVisibility::Hidden);

	RoomTitle_2->SetVisibility(ESlateVisibility::Hidden);
	curPlayers_2->SetVisibility(ESlateVisibility::Hidden);
	Divide_2->SetVisibility(ESlateVisibility::Hidden);
	maxPlayers_2->SetVisibility(ESlateVisibility::Hidden);
	EntranceButton_2->SetVisibility(ESlateVisibility::Hidden);

	RoomTitle_3->SetVisibility(ESlateVisibility::Hidden);
	curPlayers_3->SetVisibility(ESlateVisibility::Hidden);
	Divide_3->SetVisibility(ESlateVisibility::Hidden);
	maxPlayers_3->SetVisibility(ESlateVisibility::Hidden);
	EntranceButton_3->SetVisibility(ESlateVisibility::Hidden);

	curPage->SetText(FText::FromString(TEXT("1")));
	MaxPage->SetText(FText::FromString(TEXT("1")));
}