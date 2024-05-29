// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DynamicInventoryItem.h"
#include "Inventory.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPS_CPP_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IF_GetLeftHandSocketTransform(FTransform& OutTransform);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IF_GetHandSwayFloats(float& SideMove, float& MouseX, float& MouseY);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IF_GetIsAim(bool& Aim);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IF_GetStopLeftHandIK(bool& StopIK);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IF_GetWallDistance(float& Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IF_AddItemToInventory(const FDynamicInventoryItem Item, AActor* pickUp);

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_DeleteItem(AActor* ItemToDelete) = 0;

	virtual bool Server_DeleteItem_Validate(AActor* ItemToDelete) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IF_GetAnimState(UAnimInstance*& AnimState);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IF_GetAimAlpha(float& A);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IF_GetLeanBooleans(bool& Left, bool& Right);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IF_ReceiveProjectileImpact(AActor* HitActor, UActorComponent* HitComponent, const FVector HitLocation, const FVector NormalPoint);
};

class FPS_CPP_API UPlayerInterfaceImplement : public IPlayerInterface
{
	int CurrentItemSelection;
	TWeakObjectPtr<class Afps_cppCharacter> Player;
	TWeakObjectPtr<class UInventory> PlayerInventory;

public:
	int GetCurrentItemSelection() const { return CurrentItemSelection; }
	void SetCurrentItemSelection(int NewSelection) { CurrentItemSelection = NewSelection; }

	TWeakObjectPtr<class Afps_cppCharacter> GetPlayer() const { return Player; }
	void SetPlayer(TWeakObjectPtr<class Afps_cppCharacter> NewPlayer) { Player = NewPlayer; }

	TWeakObjectPtr<class UInventory> GetInventory() const { return PlayerInventory; }
	void SetInventory(TWeakObjectPtr<class UInventory> NewInventory) { PlayerInventory = NewInventory; }

	virtual void IF_GetLeftHandSocketTransform_Implementation(FTransform& OutTransform);

	virtual void IF_GetHandSwayFloats_Implementation(float& SideMove, float& MouseX, float& MouseY);

	virtual void IF_GetIsAim_Implementation(bool& Aim);

	virtual void IF_GetStopLeftHandIK_Implementation(bool& StopIK);

	virtual void IF_GetWallDistance_Implementation(float& Value);
	
	virtual bool Server_DeleteItem_Validate(AActor* ItemToDelete);
	virtual void Server_DeleteItem_Implementation(AActor* ItemToDelete);
	virtual void IF_AddItemToInventory_Implementation(const FDynamicInventoryItem Item, AActor* pickUp);

	virtual void IF_GetAnimState_Implementation(UAnimInstance*& AnimState);

	virtual void IF_GetAimAlpha_Implementation(float& A);

	virtual void IF_GetLeanBooleans_Implementation(bool& Left, bool& Right);

	virtual void IF_ReceiveProjectileImpact_Implementation(AActor* HitActor, UActorComponent* HitComponent, const FVector HitLocation, const FVector NormalPoint);
};
