// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AnimStateEnum.h"
#include "Inventory.h"
#include "ItemDataTable.h"
#include "ItemTypeEnum.h"
#include "WeaponStatsStruct.h"
#include "PaperSprite.h"
#include "DynamicInventoryItem.h"
#include "fps_cppPlayerState.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAnimStateChangedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponClassChanged);

UCLASS()
class FPS_CPP_API Afps_cppPlayerState : public APlayerState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInventory* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* DT_ItemData;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool IsDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int CurrentItemSelection;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_CurrentWeaponClass, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> CurrentWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	FWeaponStatsStruct CurrentStats;

	UPROPERTY(BlueprintAssignable)
	FAnimStateChangedEvent OnAnimStateChanged;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponClassChanged OnWeaponClassChanged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_AnimState, meta = (AllowPrivateAccess = "true"))
	EAnimStateEnum AnimState;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool StopLeftHandIK;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EItemTypeEnum WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* CurrentReloadAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPaperSprite* WeaponIcon;

public:
    Afps_cppPlayerState();

	UInventory* GetInventory() const { return InventoryComponent; }

	int GetCurrentID(int index) const { return InventoryComponent->GetCurID(index); }
	int GetCurrentBullet(int index) const { return InventoryComponent->GetCurBullet(index); }
	void ReduceCurrentBullet(int index) { InventoryComponent->ReduceBullet(index); }
	void ReloadCurrentBullet(int index) { InventoryComponent->ReloadBullet(index, CurrentStats); }

	FDynamicInventoryItem GetItem(int index) const { return InventoryComponent->GetInventory()[index]; }
	void RemoveItemAt(int index) const { InventoryComponent->GetInventory().RemoveAt(index); }

	float GetHealth() const { return Health; }
	void SetHealth(float _Health) { Health = _Health; }

	float GetMaxHealth() const { return MaxHealth; }
	void SetMaxHealth(float _MaxHealth) { MaxHealth = _MaxHealth; }

	void ReduceHealth(float Damage) { Health -= Damage; }

	bool GetIsDead() const { return IsDead; }
	void SetIsDead(bool _IsDead) { IsDead = _IsDead; }

	bool GetStopHandLeftIK() const {return StopLeftHandIK;}

	int GetCurrentItemSelection() const { return CurrentItemSelection; }
	void SetCurrentItemSelection(int CurItem) { CurrentItemSelection = CurItem; }

	TSubclassOf<AActor> GetCurrentWeaponClass() const { return CurrentWeaponClass; }

	FWeaponStatsStruct GetCurrentStats() const { return CurrentStats; }

	EItemTypeEnum GetCurrentWeaponType() const { return WeaponType; }
	void SetCurretnWeaponType(EItemTypeEnum _Weapontype) { WeaponType = _Weapontype; }

	UAnimMontage* GetCurrentReloadAnimation() const { return CurrentReloadAnimation; }
	void SetCurrentReloadAnimation(UAnimMontage* _CurrentReloadAnimation) { CurrentReloadAnimation = _CurrentReloadAnimation; }

	UPaperSprite* GetWeaponIcon() const { return WeaponIcon; }
	void SetWeaponIcon(UPaperSprite* NewIcon) { WeaponIcon = NewIcon; }

	EAnimStateEnum GetAnimState() const { return AnimState; }
	void SetAnimState(EAnimStateEnum CurAnimState) { AnimState = CurAnimState; }

	UFUNCTION()
	void OnRep_AnimState();

	UFUNCTION()
	void OnRep_CurrentWeaponClass();

	UFUNCTION()
	void StateEquipItem(int index);

	UFUNCTION(NetMulticast, Reliable)
	void SetWeaponClassMulticast(TSubclassOf<AActor> WeaponClass);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetWeaponClassServer(TSubclassOf<AActor> WeaponClass);

	UFUNCTION(NetMulticast, Reliable)
	void SetStatsToMulticast(FWeaponStatsStruct Stats);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetStatsToServer(FWeaponStatsStruct Stats);

	UFUNCTION(NetMulticast, Reliable)
	void SetAnimStateMulticast(EAnimStateEnum CurAnimState);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetAnimStateServer(EAnimStateEnum CurAnimState);

	UFUNCTION(NetMulticast, Reliable)
	void StopLeftHandIKMulticast(bool bStop);

	UFUNCTION(Server, Reliable, WithValidation)
	void StopLeftHandIKServer(bool bStop);

	void SetWeaponClassMulticast_Implementation(TSubclassOf<AActor> WBase);
	void SetWeaponClassServer_Implementation(TSubclassOf<AActor> WBase);
	bool SetWeaponClassServer_Validate(TSubclassOf<AActor> WBase);

	void StopLeftHandIKMulticast_Implementation(bool bStop);
	void StopLeftHandIKServer_Implementation(bool bStop);
	bool StopLeftHandIKServer_Validate(bool bStop);

	void SetStatsToMulticast_Implementation(FWeaponStatsStruct CurrentStats);
	void SetStatsToServer_Implementation(FWeaponStatsStruct CurrentStats);
	bool SetStatsToServer_Validate(FWeaponStatsStruct CurrentStats);

	void SetAnimStateMulticast_Implementation(EAnimStateEnum CurAnimState);
	void SetAnimStateServer_Implementation(EAnimStateEnum CurAnimState);
	bool SetAnimStateServer_Validate(EAnimStateEnum CurAnimState);


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
