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

class Afps_cppCharacter;

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

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> CurrentWeaponClass;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FWeaponStatsStruct CurrentStats;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EAnimStateEnum CurrentAnimState;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool StopLeftHandIK;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EItemTypeEnum CurrentWeaponType;

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
	void ReloadCurrentBullet(int index, int maxSize) { InventoryComponent->ReloadBullet(index, maxSize); }

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
	void SetCurrentWeaponClass(TSubclassOf<AActor> _Weapon) { CurrentWeaponClass = _Weapon; }

	FWeaponStatsStruct GetCurrentStats() const { return CurrentStats; }
	void SetCurrentStats(FWeaponStatsStruct _Stats) { CurrentStats = _Stats; }

	EItemTypeEnum GetCurrentWeaponType() const { return CurrentWeaponType; }
	void SetCurretnWeaponType(EItemTypeEnum _Weapontype) { CurrentWeaponType = _Weapontype; }

	UAnimMontage* GetCurrentReloadAnimation() const { return CurrentReloadAnimation; }
	void SetCurrentReloadAnimation(UAnimMontage* _CurrentReloadAnimation) { CurrentReloadAnimation = _CurrentReloadAnimation; }

	UPaperSprite* GetWeaponIcon() const { return WeaponIcon; }
	void SetWeaponIcon(UPaperSprite* NewIcon) { WeaponIcon = NewIcon; }

	EAnimStateEnum GetAnimState() const { return CurrentAnimState; }
	void SetAnimState(EAnimStateEnum CurAnimState) { CurrentAnimState = CurAnimState; }

	UFUNCTION(NetMulticast, Reliable)
	void SetWeaponDataMulticast(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetWeaponDataServer(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType);

	UFUNCTION(NetMulticast, Reliable)
	void StopLeftHandIKMulticast(bool bStop);

	UFUNCTION(Server, Reliable, WithValidation)
	void StopLeftHandIKServer(bool bStop);

	void StopLeftHandIKMulticast_Implementation(bool bStop);
	void StopLeftHandIKServer_Implementation(bool bStop);
	bool StopLeftHandIKServer_Validate(bool bStop);

	void SetWeaponDataMulticast_Implementation(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType);
	void SetWeaponDataServer_Implementation(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType);
	bool SetWeaponDataServer_Validate(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
