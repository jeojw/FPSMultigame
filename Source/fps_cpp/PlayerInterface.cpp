// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInterface.h"

class FPS_CPP_API UPlayerInterfaceImplement : public IPlayerInterface
{
public:
    virtual void IF_GetLeftHandSocketTransform_Implementation(FTransform& OutTransform);
    virtual void IF_GetHandSwayFloats_Implementation(float& SideMove, float& MouseX, float& MouseY);
    virtual void IF_GetIsAim_Implementation(bool& Aim);
    virtual void IF_GetStopLeftHandIK_Implementation(bool& StopIK);
    virtual void IF_GetWallDistance_Implementation(float& Value);
    virtual void IF_AddItemToInventory_Implementation(const FDynamicInventoryItem Item, AActor* pickUp);
    virtual void IF_GetAnimState_Implementation(UAnimInstance*& AnimState);
    virtual void IF_GetAimAlpha_Implementation(float& A);
    virtual void IF_GetLeanBooleans_Implementation(bool& Left, bool& Right);
    virtual void IF_ReceiveProjectileImpact_Implementation(AActor* HitActor, UActorComponent* HitComponent, const FVector HitLocation, const FVector NormalPoint);
};

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

void UPlayerInterfaceImplement::IF_AddItemToInventory_Implementation(const FDynamicInventoryItem Item, AActor* pickUp)
{

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
