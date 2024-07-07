// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"
#include "Kismet/GameplayStatics.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("ULobbyWidget::NativeConstruct called"));

	InitializePlayerController();

	if (RoomTitle_1 && curPlayers_1 && Divide_1 && maxPlayers_1 && EntranceButton_1)
	{
		RoomTitle_1->SetVisibility(ESlateVisibility::Hidden);
		curPlayers_1->SetVisibility(ESlateVisibility::Hidden);
		Divide_1->SetVisibility(ESlateVisibility::Hidden);
		maxPlayers_1->SetVisibility(ESlateVisibility::Hidden);
		EntranceButton_1->SetVisibility(ESlateVisibility::Hidden);

		EntranceButton_1->OnClicked.AddDynamic(this, &ULobbyWidget::EntranceRoom);
	}
	
	if (RoomTitle_2 && curPlayers_2 && Divide_2 && maxPlayers_2 && EntranceButton_2)
	{
		RoomTitle_2->SetVisibility(ESlateVisibility::Hidden);
		curPlayers_2->SetVisibility(ESlateVisibility::Hidden);
		Divide_2->SetVisibility(ESlateVisibility::Hidden);
		maxPlayers_2->SetVisibility(ESlateVisibility::Hidden);
		EntranceButton_2->SetVisibility(ESlateVisibility::Hidden);

		EntranceButton_2->OnClicked.AddDynamic(this, &ULobbyWidget::EntranceRoom);
	}
	
	if (RoomTitle_3 && curPlayers_3 && Divide_3 && maxPlayers_3 && EntranceButton_3)
	{
		RoomTitle_3->SetVisibility(ESlateVisibility::Hidden);
		curPlayers_3->SetVisibility(ESlateVisibility::Hidden);
		Divide_3->SetVisibility(ESlateVisibility::Hidden);
		maxPlayers_3->SetVisibility(ESlateVisibility::Hidden);
		EntranceButton_3->SetVisibility(ESlateVisibility::Hidden);

		EntranceButton_3->OnClicked.AddDynamic(this, &ULobbyWidget::EntranceRoom);
	}
	
	if (curPage && MaxPage)
	{
		curPage->SetText(FText::FromString(TEXT("1")));
		MaxPage->SetText(FText::FromString(TEXT("1")));
	}
	

	if (OptionButton)
	{
		OptionButton->OnClicked.AddDynamic(this, &ULobbyWidget::SetOptions);
	}

	if (MakeRoomButton)
	{
		MakeRoomButton->OnClicked.AddDynamic(this, &ULobbyWidget::MakeRoom);
	}

	if (LogoutButton)
	{
		LogoutButton->OnClicked.AddDynamic(this, &ULobbyWidget::LogOut);
	}
}

void ULobbyWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FVector2D& ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
	float AspectRatio = ViewportSize.X / ViewportSize.Y;

	//if (LobbyUICanvas)
	//{
	//	for (UWidget* ChildWidget : LobbyUICanvas->GetAllChildren())
	//	{
	//		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ChildWidget->Slot))
	//		{
	//			// Example: Adjust size based on aspect ratio
	//			FVector2D NewSize = FVector2D(AspectRatio * 200, AspectRatio * 50);
	//			CanvasSlot->SetSize(NewSize);

	//			// Example: Adjust position based on aspect ratio and child type
	//			FVector2D NewPosition;
	//			NewPosition = FVector2D(AspectRatio, AspectRatio);
	//			// Add more conditions for other children if necessary

	//			CanvasSlot->SetPosition(NewPosition);
	//		}
	//	}
	//}
}

void ULobbyWidget::InitializePlayerController()
{
	if (!PlayerController)
	{
		PlayerController = Cast<Afps_cppPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PlayerController)
		{
			UE_LOG(LogTemp, Log, TEXT("PlayerController successfully casted."));
			APlayerState* PlayerState = UGameplayStatics::GetPlayerState(GetWorld(), 0);
			FPSPlayerState = Cast<Afps_cppPlayerState>(PlayerState);

			if (PlayerController->PlayerState == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("PlayerState is null in PlayerController."));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("PlayerState is valid in PlayerController."));
			}

			if (FPSPlayerState)
			{
				UE_LOG(LogTemp, Log, TEXT("PlayerState successfully retrieved."));
				FPSPlayerState->OnPlayerStateUpdated.AddDynamic(this, &ULobbyWidget::UpdatePlayerInfo);
				UpdatePlayerInfo();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("PlayerState is null."));
			}
		}
	}
}

void ULobbyWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	if (InVisibility == ESlateVisibility::Visible)
	{
		if (Nickname && ProfileImage)
		{
			UpdatePlayerInfo();
		}
	}
}

void ULobbyWidget::UpdatePlayerInfo()
{
	if (FPSPlayerState)
	{
		UE_LOG(LogTemp, Log, TEXT("Updating player info."));
		if (UTexture2D* ProfileTex = FPSPlayerState->GetProfileImage())
		{
			FSlateBrush Brush;
			Brush.SetResourceObject(ProfileTex);
			Brush.ImageSize = FVector2D(ProfileTex->GetSizeX(), ProfileTex->GetSizeY());
			ProfileImage->SetBrush(Brush);
		}
		Nickname->SetText(FText::FromString(FPSPlayerState->GetPlayerNickname()));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController casting failed. 2"));
	}
}

void ULobbyWidget::LogOut()
{
	if (PlayerController->LogoutPlayer())
	{
		PlayerController->VisibleStartMenu();
	}
}

void ULobbyWidget::SetOptions()
{

}

void ULobbyWidget::MakeRoom()
{

}

void ULobbyWidget::EntranceRoom()
{

}