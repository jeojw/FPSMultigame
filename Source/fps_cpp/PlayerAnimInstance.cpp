// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "fps_cppCharacter.h"

void UPlayerAnimInstance::GetLeftHandSocketTransform(Afps_cppCharacter* Player, FTransform& OutTransform)
{
    APawn* PawnOwner = TryGetPawnOwner();
    if (PawnOwner)
    {
        Player = Cast<Afps_cppCharacter>(PawnOwner);
        if (Player)
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
    }
}

void UPlayerAnimInstance::GetHandSwayFloats(Afps_cppCharacter* Player, float& SideMove, float& MouseX, float& MouseY)
{
    APawn* PawnOwner = TryGetPawnOwner();
    if (PawnOwner)
    {
        Player = Cast<Afps_cppCharacter>(PawnOwner);
        if (Player)
        {
            SideMove = Player->GetSideMove();
            MouseX = Player->GetMouseX();
            MouseY = Player->GetMouseY();
        }
    }
}

void UPlayerAnimInstance::GetIsAim(Afps_cppCharacter* Player, bool& Aim)
{
    APawn* PawnOwner = TryGetPawnOwner();
    if (PawnOwner)
    {
        Player = Cast<Afps_cppCharacter>(PawnOwner);
        if (Player)
        {
            Aim = Player->GetIsAiming();
        }
    }
}

void UPlayerAnimInstance::GetStopLeftHandIK(Afps_cppCharacter* Player, bool& StopIK)
{
    APawn* PawnOwner = TryGetPawnOwner();
    if (PawnOwner)
    {
        Player = Cast<Afps_cppCharacter>(PawnOwner);
        if (Player)
        {
            StopIK = Player->GetIsStopLeftHandIK();
        }
    }

}

void UPlayerAnimInstance::GetWallDistance(Afps_cppCharacter* Player, float& Value)
{
    APawn* PawnOwner = TryGetPawnOwner();
    if (PawnOwner)
    {
        Player = Cast<Afps_cppCharacter>(PawnOwner);
        if (Player)
        {
            Value = Player->GetWallDistance();
        }
    }
}

void UPlayerAnimInstance::Server_DeleteItem(AActor* ItemToDelete)
{
    Server_DeleteItem_Implementation(ItemToDelete);
}

bool UPlayerAnimInstance::Server_DeleteItem_Validate(AActor* ItemToDelete)
{
    return true;
}

void UPlayerAnimInstance::Server_DeleteItem_Implementation(AActor* ItemToDelete)
{
    if (ItemToDelete)
    {
        ItemToDelete->Destroy();
    }
}

void UPlayerAnimInstance::AddItemToInventory(Afps_cppCharacter* Player, UInventory* PlayerInventory, const FDynamicInventoryItem Item, AActor* pickUp)
{
    APawn* PawnOwner = TryGetPawnOwner();
    if (PawnOwner)
    {
        Player = Cast<Afps_cppCharacter>(PawnOwner);
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
}

void UPlayerAnimInstance::GetAnimState(Afps_cppCharacter* Player, EAnimStateEnum& AnimState)
{
    APawn* PawnOwner = TryGetPawnOwner();
    if (PawnOwner)
    {
        Player = Cast<Afps_cppCharacter>(PawnOwner);
        if (Player)
        {
            AnimState = Player->GetAnimState();
        }
    }

}

void UPlayerAnimInstance::GetAimAlpha(Afps_cppCharacter* Player, float& A)
{
    APawn* PawnOwner = TryGetPawnOwner();
    if (PawnOwner)
    {
        Player = Cast<Afps_cppCharacter>(PawnOwner);
        if (Player)
        {
            A = Player->GetAimAlpha();
        }
    }

}

void UPlayerAnimInstance::GetLeanBooleans(Afps_cppCharacter* Player, bool& Left, bool& Right)
{
    APawn* PawnOwner = TryGetPawnOwner();
    if (PawnOwner)
    {
        Player = Cast<Afps_cppCharacter>(PawnOwner);
        if (Player)
        {
            Left = Player->GetLeanLeft();
            Right = Player->GetLeanRight();
        }
    }
}

void UPlayerAnimInstance::ReceiveProjectileImpact(Afps_cppCharacter* Player, AActor* HitActor, UActorComponent* HitComponent, const FVector HitLocation, const FVector NormalPoint)
{
    APawn* PawnOwner = TryGetPawnOwner();
    if (PawnOwner)
    {
        Player = Cast<Afps_cppCharacter>(PawnOwner);
        if (Player)
        {
            Player->ReceiveImpactProjectile(HitActor, HitComponent, HitLocation, NormalPoint);
        }
    }
}


