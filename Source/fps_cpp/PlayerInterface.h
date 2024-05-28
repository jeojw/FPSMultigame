// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DynamicInventoryItem.h"
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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IF_GetAnimState(UAnimInstance*& AnimState);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IF_GetAimAlpha(float& A);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IF_GetLeanBooleans(bool& Left, bool& Right);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IF_ReceiveProjectileImpact(AActor* HitActor, UActorComponent* HitComponent, const FVector HitLocation, const FVector NormalPoint);
};
