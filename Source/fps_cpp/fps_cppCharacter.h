// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PlayerInterface.h"
#include "Inventory.h"
#include "ItemDataTable.h"
#include "Weapon_Base.h"
#include "WeaponStatsStruct.h"
#include "AnimStateEnum.h"
#include "Net/UnrealNetwork.h"
#include "fps_cppCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UPlayerInterface;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAnimStateChangedEvent);

UCLASS(config = Game)
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
	TSubclassOf<AActor> bCurrentWeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* WeaponBase;

	bool bIsAttacking;

	bool bIsAiming;

	bool bStopLeftHandIK;

	FWeaponStatsStruct bCurrentStats;
	
	UAnimMontage* bCurrentReloadAnimation;

	TArray<FItemDataTable> bItemDataTable;

	TWeakObjectPtr<class UPlayerInterfaceImplement> PlayerInterface;

	EItemTypeEnum bWeaponType;

	UPROPERTY(BlueprintAssignable)
	FAnimStateChangedEvent OnAnimStateChanged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_AnimState, Category = Animation, meta = (AllowPrivateAccess = "true"))
	EAnimStateEnum bAnimState;

public:
	Afps_cppCharacter();

	bool GetIsAttacking() const { return bIsAttacking; }
	void SetIsAttacking(bool NewValue) { bIsAttacking = NewValue; }

	bool GetIsAiming() const { return bIsAiming; }
	void SetIsAiming(bool NewValue) { bIsAiming = NewValue; }

	bool GetIsStopLeftHandIK() const { return bStopLeftHandIK; }
	void SetIsStopLeftHandIK(bool StopLeftHandIK) { bStopLeftHandIK = StopLeftHandIK; }

	TWeakObjectPtr<class UPlayerInterfaceImplement> GetPlayerInterface() const { return PlayerInterface; }
	void SetPlayerInterface(TWeakObjectPtr<class UPlayerInterfaceImplement> NewPlayerInterface) { PlayerInterface = NewPlayerInterface; }

	FWeaponStatsStruct GetCurrentStats() const { return bCurrentStats; }
	void SetCurrentStats(FWeaponStatsStruct CurrentStats) { bCurrentStats = CurrentStats; }

	UAnimMontage* GetCurrentReloadAnimation() const { return bCurrentReloadAnimation; }
	void SetCurrentReloadAnimation(UAnimMontage* CurrentReloadAnimation) { bCurrentReloadAnimation = CurrentReloadAnimation; }

	TSubclassOf<AActor> GetCurrentWeaponClass() const { return bCurrentWeaponClass; }
	void SetCurrentWeaponClass(TSubclassOf<AActor> CurrentWeaponClass) { bCurrentWeaponClass = CurrentWeaponClass; }

	EItemTypeEnum GetWeaponType() const { return bWeaponType; }
	void SetWeaponType(EItemTypeEnum WeaponType) { bWeaponType = WeaponType; }

	UFUNCTION(BlueprintCallable)
	void SetWeaponClass(TSubclassOf<AActor> WBase);

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
	void SetWeaponClassMulticast(TSubclassOf<AActor> WBase);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void SetWeaponClassServer(TSubclassOf<AActor> WBase);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void SetStatsToServer(FWeaponStatsStruct CurrentStats);

	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void StopLeftHandIKServer(bool StopLeftHandIK);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void SetAnimStateServer(EAnimStateEnum AnimState);

	UFUNCTION()
	void OnRep_AnimState();

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

	void StopLeftHandIKServer_Implementation(bool StopLeftHandIK);

	void SetStatsToServer_Implementation(FWeaponStatsStruct CurrentStats);
	bool SetStatsToServer_Validate(FWeaponStatsStruct CurrentStats);

	void SetAnimStateServer_Implementation(EAnimStateEnum AnimState);
	bool SetAnimStateServer_Validate(EAnimStateEnum AnimState);

	void DeleteItemServer_Implementation(AActor* DeleteItem);
	bool DeleteItemServer_Validate(AActor* DeleteItem);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

