// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UImage* ProfileImage;

	UPROPERTY(meta = (BindWidget))
	UButton* OptionButton;

	UPROPERTY(meta = (BindWidget))
	UButton* MakeRoomButton;

	UPROPERTY(meta = (BindWidget))
	UButton* LogoutButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Nickname;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoomTitle_1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* curPlayers_1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Divide_1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* maxPlayers_1;

	UPROPERTY(meta = (BindWidget))
	UButton* EntranceButton_1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoomTitle_2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* curPlayers_2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Divide_2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* maxPlayers_2;

	UPROPERTY(meta = (BindWidget))
	UButton* EntranceButton_2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoomTitle_3;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* curPlayers_3;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Divide_3;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* maxPlayers_3;

	UPROPERTY(meta = (BindWidget))
	UButton* EntranceButton_3;

	UPROPERTY(meta = (BindWidget))
	UButton* NextButton;

	UPROPERTY(meta = (BindWidget))
	UButton* PreButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* curPage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxPage;


protected:
	virtual void NativeConstruct() override;

public:

	
};
