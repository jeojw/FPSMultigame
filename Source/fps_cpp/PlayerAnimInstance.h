// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "fps_cppCharacter.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	void GetLeftHandSocketTransform(Afps_cppCharacter* Player, FTransform& OutTransform);

	void GetHandSwayFloats(Afps_cppCharacter* Player, float& SideMove, float& MouseX, float& MouseY);

	void GetIsAim(Afps_cppCharacter* Player, bool& Aim);

	void GetStopLeftHandIK(Afps_cppCharacter* Player, bool& StopIK);

	void GetWallDistance(Afps_cppCharacter* Player, float& Value);

	void Server_DeleteItem(AActor* ItemToDelete);
	bool Server_DeleteItem_Validate(AActor* ItemToDelete);
	void Server_DeleteItem_Implementation(AActor* ItemToDelete);

	void AddItemToInventory(Afps_cppCharacter* Player, UInventory* PlayerInventory, const FDynamicInventoryItem Item, AActor* pickUp);

	void GetAnimState(Afps_cppCharacter* Player, EAnimStateEnum& AnimState);

	void GetAimAlpha(Afps_cppCharacter* Player, float& A);

	void GetLeanBooleans(Afps_cppCharacter* Player, bool& Left, bool& Right);

	void ReceiveProjectileImpact(Afps_cppCharacter* Player, AActor* HitActor, UActorComponent* HitComponent, const FVector HitLocation, const FVector NormalPoint);
};
