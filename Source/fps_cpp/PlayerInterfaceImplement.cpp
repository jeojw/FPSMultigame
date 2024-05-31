// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerInterfaceImplement.h"
#include "Math/Transform.h"
#include "Math/Vector.h"
#include "Math/Quat.h"
#include "Math/Rotator.h"
#include "Animation/AnimInstance.h"
#include "fps_cppCharacter.h"

UPlayerInterfaceImplement::UPlayerInterfaceImplement()
{
    CurrentItemSelection = 0; // 기본값 초기화
}

void UPlayerInterfaceImplement::IF_GetLeftHandSocketTransform_Implementation(FTransform& OutTransform)
{
    if (Player)
    {
        AActor* ChildActor = Player->GetWeaponBase()->GetChildActor();
        if (ChildActor && !ChildActor->IsPendingKillEnabled())
        {
            AWeapon_Base* WeaponBase = Cast<AWeapon_Base>(ChildActor);
            if (WeaponBase)
            {
                USkeletalMeshComponent* WeaponMesh = WeaponBase->GetSkeletalMeshComponent();
                if (WeaponMesh)
                {
                    FTransform WeaponSocketTransform = WeaponMesh->GetSocketTransform(FName("LHIK"), RTS_World);
                    FVector OutPosition;
                    FRotator OutRotation;
                    WeaponMesh->TransformToBoneSpace(FName("hand_r"), WeaponSocketTransform.GetLocation(), FRotator(WeaponSocketTransform.GetRotation()), OutPosition, OutRotation);

                    FTransform ReturnTransform;
                    ReturnTransform.SetLocation(OutPosition);
                    ReturnTransform.SetRotation(FQuat(OutRotation));
                    ReturnTransform.SetScale3D(FVector(1, 1, 1));

                    OutTransform = ReturnTransform;
                }
            }
        }
    }
}

void UPlayerInterfaceImplement::IF_GetHandSwayFloats_Implementation(float& SideMove, float& MouseX, float& MouseY)
{
    if (Player)
    {
        SideMove = Player->GetSideMove();
        MouseX = Player->GetMouseX();
        MouseY = Player->GetMouseY();
    }
}

void UPlayerInterfaceImplement::IF_GetIsAim_Implementation(bool& Aim)
{
    if (Player)
    {
        Aim = Player->GetIsAiming();
    }
}

void UPlayerInterfaceImplement::IF_GetStopLeftHandIK_Implementation(bool& StopIK)
{
    if (Player)
    {
        StopIK = Player->GetIsStopLeftHandIK();
    }
    
}

void UPlayerInterfaceImplement::IF_GetWallDistance_Implementation(float& Value)
{
    if (Player)
    {
        Value = Player->GetWallDistance();
    }
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
    if (Player && PlayerInventory)
    {
        if (Player->IsLocallyControlled() && PlayerInventory->GetInventory().Num() <= PlayerInventory->MaxItemCount) {
            PlayerInventory->GetInventory().Add(Item);
            if (pickUp) {
                Server_DeleteItem(pickUp);
                Player->SetCurrentItemSelection(0);
                Player->EquiptItem();
            }
        }
    }
}

void UPlayerInterfaceImplement::IF_GetAnimState_Implementation(EAnimStateEnum& AnimState)
{
    if (Player)
    {
        AnimState = Player->GetAnimState();
    }
    
}

void UPlayerInterfaceImplement::IF_GetAimAlpha_Implementation(float& A)
{
    if (Player)
    {
        A = Player->GetAimAlpha();
    }
   
}

void UPlayerInterfaceImplement::IF_GetLeanBooleans_Implementation(bool& Left, bool& Right)
{
    if (Player)
    {
        Left = Player->GetLeanLeft();
        Right = Player->GetLeanRight();
    }
}

void UPlayerInterfaceImplement::IF_ReceiveProjectileImpact_Implementation(AActor* HitActor, UActorComponent* HitComponent, const FVector HitLocation, const FVector NormalPoint)
{
    if (Player)
    {
        Player->ReceiveImpactProjectile(HitActor, HitComponent, HitLocation, NormalPoint);
    }
}

