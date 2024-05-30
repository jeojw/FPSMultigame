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
#include "PickUpBase.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	bool bStopLeftHandIK;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	bool bLeanLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	bool bLeanRight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float bAimAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float bWallDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HandSway, meta = (AllowPrivateAccess = "true"))
	float bSideMove;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HandSway, meta = (AllowPrivateAccess = "true"))
	float bMouseX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HandSway, meta = (AllowPrivateAccess = "true"))
	float bMouseY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int bCurrentItemSelection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTimerHandle bFireCooldownTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APickUpBase> bCurrentWeaponPickUpClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FWeaponStatsStruct bCurrentStats;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* bCurrentReloadAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* DT_ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TScriptInterface<IPlayerInterface> PlayerInterface;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
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

	bool GetLeanLeft() const { return bLeanLeft; }
	bool GetLeanRight() const { return bLeanRight; }
	void SetLeanLeft(bool LeanLeft) { bLeanLeft = LeanLeft; }
	void SetLeanRight(bool LeanRight) { bLeanRight = LeanRight; }

	float GetAimAlpha() const { return bAimAlpha; }
	void SetAimAlpha(float AimAlpha) { bAimAlpha = AimAlpha; }

	float GetWallDistance() const { return bWallDistance; }
	void SetWallDistance(float WallDistance) { bWallDistance = WallDistance; }

	float GetSideMove() const { return bSideMove; }
	void SetSideMove(float SideMove) { bSideMove = SideMove; }

	float GetMouseX() const { return bMouseX; }
	void SetMouseX(float MouseX) { bMouseX = MouseX; }

	float GetMouseY() const { return bMouseY; }
	void SetMouseY(float MouseY) { bMouseY = MouseY; }

	UChildActorComponent* GetWeaponBase() const { return WeaponBase; }
	void SetWeaponBase(UChildActorComponent* _WeaponBase) { WeaponBase = _WeaponBase; }

	EAnimStateEnum GetAnimState() const { return bAnimState; }
	void SetAnimState(EAnimStateEnum AnimState) { bAnimState = AnimState; }

	TScriptInterface<IPlayerInterface> GetPlayerInterface() const { return PlayerInterface; }
	void SetPlayerInterface(TScriptInterface<IPlayerInterface> NewPlayerInterface) { PlayerInterface = NewPlayerInterface; }

	FWeaponStatsStruct GetCurrentStats() const { return bCurrentStats; }
	void SetCurrentStats(FWeaponStatsStruct CurrentStats) { bCurrentStats = CurrentStats; }

	UAnimMontage* GetCurrentReloadAnimation() const { return bCurrentReloadAnimation; }
	void SetCurrentReloadAnimation(UAnimMontage* CurrentReloadAnimation) { bCurrentReloadAnimation = CurrentReloadAnimation; }

	TSubclassOf<AActor> GetCurrentWeaponClass() const { return bCurrentWeaponClass; }
	void SetCurrentWeaponClass(TSubclassOf<AActor> CurrentWeaponClass) { bCurrentWeaponClass = CurrentWeaponClass; }

	EItemTypeEnum GetWeaponType() const { return bWeaponType; }
	void SetWeaponType(EItemTypeEnum WeaponType) { bWeaponType = WeaponType; }

	int GetCurrentItemSelection() const { return bCurrentItemSelection; }
	void SetCurrentItemSelection(int CurItem) { bCurrentItemSelection = CurItem; }

	UFUNCTION(BlueprintCallable)
	void SetWeaponClass(TSubclassOf<AActor> WBase);

	void DelayedFunction();

public:
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

	UFUNCTION(BlueprintCallable)
	void EquiptItem();

	UFUNCTION(BlueprintCallable)
	void ReceiveImpactProjectile(AActor* actor, UActorComponent* comp, FVector Loc, FVector Normal);

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
	void PlayAnimMontageMulticast(UAnimMontage* AnimMontage);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void PlayAnimMontageServer(UAnimMontage* AnimMontage);

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

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void SpawnBulletHole(FTransform SpawnTransform);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void SpawnPickupActorServer(FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, FDynamicInventoryItem Item, TSubclassOf<class APickUpBase> Class);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SprintServer(float MaxWalkSpeed);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void DeleteItemServer(AActor* DeleteItem);

	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void ApplyDamageServer(AActor* DamageActor, float BaseDamage, AActor* DamageCauser);

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

	void SpawnBulletHole_Implementation(FTransform SpawnTransform);
	bool SpawnBulletHole_Validate(FTransform SpawnTransform);

	void PlayAnimMontageMulticast_Implementation(UAnimMontage* AnimMontage);
	void PlayAnimMontageServer_Implementation(UAnimMontage* AnimMontage);
	bool PlayAnimMontageServer_Validate(UAnimMontage* AnimMontage);

	void SetAnimStateServer_Implementation(EAnimStateEnum AnimState);
	bool SetAnimStateServer_Validate(EAnimStateEnum AnimState);

	void SpawnPickupActorServer_Implementation(FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, FDynamicInventoryItem Item, TSubclassOf<class APickUpBase> Class);
	bool SpawnPickupActorServer_Validate(FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, FDynamicInventoryItem Item, TSubclassOf<class APickUpBase> Class);

	void DeleteItemServer_Implementation(AActor* DeleteItem);
	bool DeleteItemServer_Validate(AActor* DeleteItem);

	void ApplyDamageServer_Implementation(AActor* DamageActor, float BaseDamage, AActor* DamageCauser);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

