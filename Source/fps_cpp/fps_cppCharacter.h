// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PlayerInterface.h"
#include "Inventory.h"
#include "ItemDataTable.h"
#include "Weapon_Base.h"
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

	UPROPERTY(Replicated)
	TSubclassOf<AActor> CurrentWeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* WeaponBase;

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

	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void PlaySoundAtLocationMulticast(FVector Location, USoundBase* Sound);

	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void PlaySoundAtLocationServer(FVector Location, USoundBase* Sound);

	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void SpawnEmitterAtLocationMulticast(UParticleSystem* EmitterTemplate, FVector Location, FRotator Rotation, FVector Scale);

	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void SpawnEmitterAtLocationServer(UParticleSystem* EmitterTemplate, FVector Location, FRotator Rotation, FVector Scale);

	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void SpawnActorToServer(TSubclassOf<AActor> Class, FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetWeaponClassMulticast(TSubclassOf<AActor> WBas);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void SetWeaponClassServer(TSubclassOf<AActor> WBase);

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

	void PlaySoundAtLocationMulticast_Implementation(FVector Location, USoundBase* Sound);
	void PlaySoundAtLocationServer_Implementation(FVector Location, USoundBase* Sound);

	void SpawnEmitterAtLocationMulticast_Implementation(UParticleSystem* EmitterTemplate, FVector Location, FRotator Rotation, FVector Scale);
	void SpawnEmitterAtLocationServer_Implementation(UParticleSystem* EmitterTemplate, FVector Location, FRotator Rotation, FVector Scale);

	void SpawnActorToServer_Implementation(TSubclassOf<AActor> Class, FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride);

	void SetWeaponClassMulticast_Implementation(TSubclassOf<AActor> WBase);
	void SetWeaponClassServer_Implementation(TSubclassOf<AActor> WBase);
	bool SetWeaponClassServer_Validate(TSubclassOf<AActor> WBase);

	void SprintServer_Implementation(float MaxWalkSpeed);
	bool SprintServer_Validate(float MaxWalkSpeed);

	void DeleteItemServer_Implementation(AActor* DeleteItem);
	bool DeleteItemServer_Validate(AActor* DeleteItem);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};

