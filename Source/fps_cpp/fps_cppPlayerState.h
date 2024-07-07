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
#include "Engine/Texture2D.h"
#include "fps_cppPlayerState.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerStateUpdated);

UCLASS()
class FPS_CPP_API Afps_cppPlayerState : public APlayerState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInventory* InventoryComponent;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool IsDead;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Player_ID, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString Player_ID;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_PlayerNickname, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString Player_Nickname;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_ProfileImage, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UTexture2D* ProfileImage;

	UFUNCTION()
	void OnRep_Player_ID();

	UFUNCTION()
	void OnRep_PlayerNickname();

	UFUNCTION()
	void OnRep_ProfileImage();

public:
    Afps_cppPlayerState();

	FOnPlayerStateUpdated OnPlayerStateUpdated;

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

	UFUNCTION(BlueprintCallable, Category = "Profile")
	FString GetPlayer_Id() const { return Player_ID; }
	void SetPlayer_Id(const FString& _id);

	UFUNCTION(BlueprintCallable, Category = "Profile")
	FString GetPlayerNickname() const { return Player_Nickname; }
	void SetPlayerNickname(const FString& _nickname);

	UFUNCTION(BlueprintCallable, Category = "Profile")
	UTexture2D* GetProfileImage() const { return ProfileImage; }
	void SetProfileImage(UTexture2D* _img);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetPlayerInfoServer(const FString& _playerid, const FString& _nickname, UTexture2D* _profileImage);

	UFUNCTION(NetMulticast, Reliable)
	void SetPlayerInfoMulticast(const FString& _playerid, const FString& _nickname, UTexture2D* _profileImage);

	void SetPlayerInfoServer_Implementation(const FString& _playerid, const FString& _nickname, UTexture2D* _profileImage);
	bool SetPlayerInfoServer_Validate(const FString& _playerid, const FString& _nickname, UTexture2D* _profileImage);
	void SetPlayerInfoMulticast_Implementation(const FString& _playerid, const FString& _nickname, UTexture2D* _profileImage);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
