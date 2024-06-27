// Fill out your copyright notice in the Description page of Project Settings.


#include "fps_cppPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "fps_cppPlayerController.h"
#include "Net/UnrealNetwork.h"

Afps_cppPlayerState::Afps_cppPlayerState()
{
	InventoryComponent = CreateDefaultSubobject<UInventory>(TEXT("InventoryComponent"));

	AnimState = EAnimStateEnum::Rifle;
	CurrentItemSelection = 0;
	DT_ItemData = LoadObject<UDataTable>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/inventory/ItemData/DT_ItemData"));

	MaxHealth = 200.0f;
	Health = 200.0f;
	IsDead = false;
}

void Afps_cppPlayerState::StateEquipItem(int index)
{
	if (!IsDead)
	{
		Afps_cppPlayerController* Controller = Cast<Afps_cppPlayerController>(GetOwner());
		if (Controller && Controller->IsLocalController()) {
			if (!InventoryComponent) {
				UE_LOG(LogTemp, Error, TEXT("Inventory is nullptr"));
				return;
			}

			int CurrentSelection = index;
			if (!InventoryComponent->GetInventory().IsValidIndex(CurrentSelection)) {
				UE_LOG(LogTemp, Error, TEXT("Invalid inventory index: %d"), CurrentSelection);
				return;
			}

			int id = InventoryComponent->GetInventory()[CurrentSelection].ID;
			FString fname = FString::FromInt(id);
			if (DT_ItemData) {
				TArray<FName> RowNames = DT_ItemData->GetRowNames();
				for (int i = 0; i < RowNames.Num(); i++) {
					if (RowNames[i] == fname)
					{
						FItemDataTable* data = DT_ItemData->FindRow<FItemDataTable>(RowNames[i], RowNames[i].ToString());
						if (data)
						{
							if (HasAuthority())
							{
								SetWeaponClassMulticast(data->WeaponClass);
								SetStatsToMulticast(data->Stats);
								SetAnimStateMulticast(data->AnimState);
							}
							else
							{
								SetWeaponClassServer(data->WeaponClass);
								SetStatsToServer(data->Stats);
								SetAnimStateServer(data->AnimState);
							}
							if (data->AnimState == EAnimStateEnum::Melee)
							{
								if (HasAuthority())
								{
									StopLeftHandIKMulticast(true);
								}
								else
								{
									StopLeftHandIKServer(true);
								}
							}
							else
							{
								if (HasAuthority())
								{
									StopLeftHandIKMulticast(false);
								}
								else
								{
									StopLeftHandIKServer(false);
								}
							}
							SetCurrentReloadAnimation(data->ReloadAnimation);
							SetWeaponIcon(data->icon);
							SetCurretnWeaponType(data->Type);

							CurrentWeaponClass = data->WeaponClass;
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("Failed to find data for item %s"), *fname);
						}
						break;
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to load item data table"));
			}
		}
	}
}

void Afps_cppPlayerState::SetWeaponClassMulticast_Implementation(TSubclassOf<AActor> WeaponClass)
{
	CurrentWeaponClass = WeaponClass;
	OnRep_CurrentWeaponClass();
}

void Afps_cppPlayerState::SetWeaponClassServer_Implementation(TSubclassOf<AActor> WeaponClass)
{
	SetWeaponClassMulticast(WeaponClass);
}

bool Afps_cppPlayerState::SetWeaponClassServer_Validate(TSubclassOf<AActor> WeaponClass)
{
	return true;
}

void Afps_cppPlayerState::SetStatsToMulticast_Implementation(FWeaponStatsStruct Stats)
{
	CurrentStats = Stats;
}

void Afps_cppPlayerState::SetStatsToServer_Implementation(FWeaponStatsStruct Stats)
{
	SetStatsToMulticast(Stats);
}

bool Afps_cppPlayerState::SetStatsToServer_Validate(FWeaponStatsStruct Stats)
{
	return true;
}

void Afps_cppPlayerState::SetAnimStateMulticast_Implementation(EAnimStateEnum CurAnimState)
{
	AnimState = CurAnimState;
	OnRep_AnimState();
}

void Afps_cppPlayerState::SetAnimStateServer_Implementation(EAnimStateEnum CurAnimState)
{
	SetAnimStateMulticast(CurAnimState);
}

bool Afps_cppPlayerState::SetAnimStateServer_Validate(EAnimStateEnum CurAnimState)
{
	return true;
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

void Afps_cppPlayerState::OnRep_AnimState()
{
	OnAnimStateChanged.Broadcast();
}

void Afps_cppPlayerState::OnRep_CurrentWeaponClass()
{
	OnWeaponClassChanged.Broadcast();
}

void Afps_cppPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(Afps_cppPlayerState, Health);
	DOREPLIFETIME(Afps_cppPlayerState, IsDead);

	DOREPLIFETIME(Afps_cppPlayerState, CurrentWeaponClass);
	DOREPLIFETIME(Afps_cppPlayerState, CurrentStats);
	DOREPLIFETIME(Afps_cppPlayerState, AnimState);

	DOREPLIFETIME(Afps_cppPlayerState, StopLeftHandIK);
}