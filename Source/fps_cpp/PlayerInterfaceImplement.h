// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInterface.h"
#include "Inventory.h"
#include "ItemDataTable.h"
#include "AnimStateEnum.h"
#include "fps_cppCharacter.h"
#include "Components/SkinnedMeshComponent.h"
#include "PlayerInterfaceImplement.generated.h"
/**
 * 
 */
UCLASS()
class FPS_CPP_API UPlayerInterfaceImplement : public UObject, public IPlayerInterface
{
	GENERATED_BODY()

	int CurrentItemSelection;
	TWeakObjectPtr<class Afps_cppCharacter> Player;
	TWeakObjectPtr<class UInventory> PlayerInventory;

public:
	UPlayerInterfaceImplement();

	TWeakObjectPtr<class Afps_cppCharacter> GetPlayer() const { return Player; }
	void SetPlayer(TWeakObjectPtr<class Afps_cppCharacter> NewPlayer) { Player = NewPlayer; }

	TWeakObjectPtr<class UInventory> GetInventory() const { return PlayerInventory; }
	void SetInventory(TWeakObjectPtr<class UInventory> NewInventory) { PlayerInventory = NewInventory; }

	virtual void IF_GetLeftHandSocketTransform_Implementation(FTransform& OutTransform) override;

	virtual void IF_GetHandSwayFloats_Implementation(float& SideMove, float& MouseX, float& MouseY) override;

	virtual void IF_GetIsAim_Implementation(bool& Aim) override;

	virtual void IF_GetStopLeftHandIK_Implementation(bool& StopIK) override;

	virtual void IF_GetWallDistance_Implementation(float& Value) override;

	virtual bool Server_DeleteItem_Validate(AActor* ItemToDelete);
	virtual void Server_DeleteItem_Implementation(AActor* ItemToDelete) override;
	virtual void IF_AddItemToInventory_Implementation(const FDynamicInventoryItem Item, AActor* pickUp) override;

	virtual void IF_GetAnimState_Implementation(EAnimStateEnum& AnimState) override;

	virtual void IF_GetAimAlpha_Implementation(float& A) override;

	virtual void IF_GetLeanBooleans_Implementation(bool& Left, bool& Right) override;

	virtual void IF_ReceiveProjectileImpact_Implementation(AActor* HitActor, UActorComponent* HitComponent, const FVector HitLocation, const FVector NormalPoint) override;
};
