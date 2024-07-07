// Fill out your copyright notice in the Description page of Project Settings.


#include "fps_cppPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "fps_cppPlayerController.h"
#include "fps_cppCharacter.h"
#include "Net/UnrealNetwork.h"

Afps_cppPlayerState::Afps_cppPlayerState()
{
	bReplicates = true;

	InventoryComponent = CreateDefaultSubobject<UInventory>(TEXT("InventoryComponent"));

	MaxHealth = 200.0f;
	Health = 200.0f;
	IsDead = false;
}

void Afps_cppPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(Afps_cppPlayerState, InventoryComponent);
	DOREPLIFETIME(Afps_cppPlayerState, MaxHealth);
	DOREPLIFETIME(Afps_cppPlayerState, Health);
	DOREPLIFETIME(Afps_cppPlayerState, IsDead);

	DOREPLIFETIME(Afps_cppPlayerState, Player_ID);
	DOREPLIFETIME(Afps_cppPlayerState, Player_Nickname);
	DOREPLIFETIME(Afps_cppPlayerState, ProfileImage);
}

void Afps_cppPlayerState::SetPlayerInfoServer_Implementation(const FString& _playerid, const FString& _nickname, UTexture2D* _profileImage)
{
	SetPlayerInfoMulticast(_playerid, _nickname, _profileImage);
}
bool Afps_cppPlayerState::SetPlayerInfoServer_Validate(const FString& _playerid, const FString& _nickname, UTexture2D* _profileImage)
{
	return true;
}
void Afps_cppPlayerState::SetPlayerInfoMulticast_Implementation(const FString& _playerid, const FString& _nickname, UTexture2D* _profileImage)
{
	SetPlayer_Id(_playerid);
	SetPlayerNickname(_nickname);
	SetProfileImage(_profileImage);
}

void Afps_cppPlayerState::SetPlayer_Id(const FString& NewID)
{
	Player_ID = NewID;
	OnRep_Player_ID();
}

void Afps_cppPlayerState::SetPlayerNickname(const FString& NewNickname)
{
	Player_Nickname = NewNickname;
	OnRep_PlayerNickname();
}

void Afps_cppPlayerState::SetProfileImage(UTexture2D* NewProfileImage)
{
	ProfileImage = NewProfileImage;
	OnRep_ProfileImage();
}

void Afps_cppPlayerState::OnRep_Player_ID()
{
	OnPlayerStateUpdated.Broadcast();
}

void Afps_cppPlayerState::OnRep_PlayerNickname()
{
	OnPlayerStateUpdated.Broadcast();
}

void Afps_cppPlayerState::OnRep_ProfileImage()
{
	OnPlayerStateUpdated.Broadcast();
}