// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerInterfaceImplement.h"
#include "Math/Transform.h"
#include "Math/Vector.h"
#include "Math/Quat.h"
#include "Math/Rotator.h"

void UPlayerInterfaceImplement::IF_GetLeftHandSocketTransform_Implementation(FTransform& OutTransform)
{
    if (Player->GetWeaponBase()->GetChildActor() && !Player->GetWeaponBase()->GetChildActor()->IsPendingKillEnabled())
    {
        USkeletalMeshComponent* WeaponMesh = Cast<USkeletalMeshComponent>(Player->GetWeaponBase());
        FTransform WeaponSocketTransform = WeaponMesh->GetSocketTransform(FName("LHIK"));
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

void UPlayerInterfaceImplement::IF_GetHandSwayFloats_Implementation(float& SideMove, float& MouseX, float& MouseY)
{
    SideMove = Player->GetSideMove();
    MouseX = Player->GetMouseX();
    MouseY = Player->GetMouseY();
}

void UPlayerInterfaceImplement::IF_GetIsAim_Implementation(bool& Aim)
{
    Aim = Player->GetIsAiming();
}

void UPlayerInterfaceImplement::IF_GetStopLeftHandIK_Implementation(bool& StopIK)
{
    StopIK = Player->GetIsStopLeftHandIK();
}

void UPlayerInterfaceImplement::IF_GetWallDistance_Implementation(float& Value)
{
    Value = Player->GetWallDistance();
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
    if (Player.IsValid() && PlayerInventory.IsValid()) {
        if (Player.Get()->IsLocallyControlled() && PlayerInventory->GetInventory().Num() <= PlayerInventory->MaxItemCount) {
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
    AnimState = Player->GetAnimState();
}

void UPlayerInterfaceImplement::IF_GetAimAlpha_Implementation(float& A)
{
    A = Player->GetAimAlpha();
}

void UPlayerInterfaceImplement::IF_GetLeanBooleans_Implementation(bool& Left, bool& Right)
{
    Left = Player->GetLeanLeft();
    Right = Player->GetLeanRight();
}

void UPlayerInterfaceImplement::IF_ReceiveProjectileImpact_Implementation(AActor* HitActor, UActorComponent* HitComponent, const FVector HitLocation, const FVector NormalPoint)
{
    Player->ReceiveImpactProjectile(HitActor, HitComponent, HitLocation, NormalPoint);
}

