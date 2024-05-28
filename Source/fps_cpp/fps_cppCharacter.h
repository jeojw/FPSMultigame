// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PlayerInterface.h"
#include "Inventory.h"
#include "ItemDataTable.h"
#include "fps_cppCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class Afps_cppCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Custom, meta = (AllowPrivateAccess = "true"))
	UInventory* Inventory;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DropItemAction;

	bool bIsAttacking;

	bool bIsAiming;

	TWeakObjectPtr<class UPlayerInterfaceImplement> PlayerInterface;

public:
	Afps_cppCharacter();

	bool GetIsAttacking() const { return bIsAttacking; }
	void SetIsAttacking(bool NewValue) { bIsAttacking = NewValue; }

	bool GetIsAiming() const { return bIsAiming; }
	void SetIsAiming(bool NewValue) { bIsAiming = NewValue; }

	TWeakObjectPtr<class UPlayerInterfaceImplement> GetPlayerInterface() const { return PlayerInterface; }
	void SetPlayerInterface(TWeakObjectPtr<class UPlayerInterfaceImplement> NewPlayerInterface) { PlayerInterface = NewPlayerInterface; }

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Sprint();

	void StopSprint();
			
	void Fire();

	void RifleFire();

	void PistolFire();

	void StopFire();

	void Aiming();

	void StopAiming();

	void Reload();

	void DropItem();

	UFUNCTION()
	void EquiptItem();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SprintServer(float MaxWalkSpeed);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void DeleteItemServer(AActor* DeleteItem);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void SprintServer_Implementation(float MaxWalkSpeed);
	bool SprintServer_Validate(float MaxWalkSpeed);

	void DeleteItemServer_Implementation(AActor* DeleteItem);
	bool DeleteItemServer_Validate(AActor* DeleteItem);
};

