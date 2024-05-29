// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInterface.h"

void UPlayerInterfaceImplement::IF_GetLeftHandSocketTransform_Implementation(FTransform& OutTransform)
{

}

void UPlayerInterfaceImplement::IF_GetHandSwayFloats_Implementation(float& SideMove, float& MouseX, float& MouseY)
{

}

void UPlayerInterfaceImplement::IF_GetIsAim_Implementation(bool& Aim)
{

}

void UPlayerInterfaceImplement::IF_GetStopLeftHandIK_Implementation(bool& StopIK)
{

}

void UPlayerInterfaceImplement::IF_GetWallDistance_Implementation(float& Value)
{

}

bool UPlayerInterfaceImplement::Server_DeleteItem_Validate(AActor* ItemToDelete)
{
    return true;
}

void UPlayerInterfaceImplement::Server_DeleteItem_Implementation(AActor* ItemToDelete)
{
    if (ItemToDelete)
    {
        ItemToDelete->Destroy();
    }
}

void UPlayerInterfaceImplement::IF_AddItemToInventory_Implementation(const FDynamicInventoryItem Item, AActor* pickUp)
{
    /*if (Player.IsValid() && PlayerInventory.IsValid()) {
        if (Player.Get()->IsLocallyControlled() && PlayerInventory->Inventory.Num() <= PlayerInventory->MaxItemCount) {
            PlayerInventory->Inventory.Add(Item);

            if (pickUp) {
                Server_DeleteItem_Implementation(pickUp);
            }

            CurrentItemSelection = 0;
        }
    }*/
    
}

void UPlayerInterfaceImplement::IF_GetAnimState_Implementation(UAnimInstance*& AnimState)
{

}

void UPlayerInterfaceImplement::IF_GetAimAlpha_Implementation(float& A)
{

}

void UPlayerInterfaceImplement::IF_GetLeanBooleans_Implementation(bool& Left, bool& Right)
{

}

void UPlayerInterfaceImplement::IF_ReceiveProjectileImpact_Implementation(AActor* HitActor, UActorComponent* HitComponent, const FVector HitLocation, const FVector NormalPoint)
{

}

// Add default functionality here for any IPlayerInterface functions that are not pure virtual.
