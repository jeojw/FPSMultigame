// Fill out your copyright notice in the Description page of Project Settings.


#include "fps_cppPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "fps_cppPlayerController.h"
#include "fps_cppCharacter.h"
#include "Net/UnrealNetwork.h"

Afps_cppPlayerState::Afps_cppPlayerState()
{
	InventoryComponent = CreateDefaultSubobject<UInventory>(TEXT("InventoryComponent"));

	CurrentAnimState = EAnimStateEnum::Rifle;
	CurrentItemSelection = 0;
	DT_ItemData = LoadObject<UDataTable>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/inventory/ItemData/DT_ItemData"));

	MaxHealth = 200.0f;
	Health = 200.0f;
	IsDead = false;
}

void Afps_cppPlayerState::SetWeaponDataServer_Implementation(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType)
{
	SetWeaponDataMulticast(WeaponClass, Stats, AnimState, bStop, WeaponType);
}

bool Afps_cppPlayerState::SetWeaponDataServer_Validate(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType)
{
	return true;
}

void Afps_cppPlayerState::SetWeaponDataMulticast_Implementation(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType)
{
	CurrentWeaponClass = WeaponClass;
	CurrentStats = Stats;
	CurrentAnimState = AnimState;
	StopLeftHandIK = bStop;
	CurrentWeaponType = WeaponType;
	UE_LOG(LogTemp, Log, TEXT("Weapon data set: %s"), *WeaponClass->GetName());
}

void Afps_cppPlayerState::StopLeftHandIKMulticast_Implementation(bool bStop)
{
	StopLeftHandIK = bStop;
}

void Afps_cppPlayerState::StopLeftHandIKServer_Implementation(bool bStop)
{
	StopLeftHandIKMulticast(bStop);
}

bool Afps_cppPlayerState::StopLeftHandIKServer_Validate(bool bStop)
{
	return true;
}

void Afps_cppPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(Afps_cppPlayerState, MaxHealth);
	DOREPLIFETIME(Afps_cppPlayerState, Health);
	DOREPLIFETIME(Afps_cppPlayerState, IsDead);

	DOREPLIFETIME(Afps_cppPlayerState, CurrentWeaponClass);
	DOREPLIFETIME(Afps_cppPlayerState, CurrentStats);
	DOREPLIFETIME(Afps_cppPlayerState, CurrentAnimState);
	DOREPLIFETIME(Afps_cppPlayerState, StopLeftHandIK);
	DOREPLIFETIME(Afps_cppPlayerState, CurrentWeaponType);
}