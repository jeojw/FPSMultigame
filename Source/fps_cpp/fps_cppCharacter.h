﻿// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PlayerInterface.h"
#include "Inventory.h"
#include "ItemDataTable.h"
#include "Net/UnrealNetwork.h"
#include "PickUpBase.h"
#include "Shell_Base.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "ProjectileBullet.h"
#include "Sound/SoundCue.h"
#include "Components/TimelineComponent.h"
#include "PaperSprite.h"
#include "Blueprint/UserWidget.h"
#include "fps_cppPlayerState.h"
#include "fps_cppCharacter.generated.h"

class UPlayerInterfaceImplement;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class Afps_cppCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FPSMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	Afps_cppPlayerState* FPSPlayerState;
	
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
	UInputAction* SwitchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeanAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DropItemAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* WeaponBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* FPSWeaponBase;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsReloading;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bStopLeftHandIK;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bSwitching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bWalking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bSprinting;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bSoundPlaying;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bJumping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bLeanLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TScriptInterface<IPlayerInterface> PlayerInterface;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform bLeftHandSocketTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTimerHandle bTimerHandle_CheckWallTick;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTimerHandle bFireRateTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTimerHandle bShellEjectTimer;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FTimeline bRecoilTimeline;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FTimeline bAimTimeline;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* bRecoilCurve;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* bAimCurve;

	USoundCue* RifleImpactSoundCue;
	USoundCue* RifleSurfaceImpactSoundCue;
	USoundCue* PistolSurfaceImpactSoundCue;
	UParticleSystem* MuzzleFlashParticleSystem;
	UParticleSystem* MetalImpactParticleSystem;
	UParticleSystem* StoneImpactParticleSystem;

	USoundCue* MetalWalkSoundCue;
	USoundCue* MetalRunSoundCue;
	USoundCue* MetalJumpSoundCue;
	USoundCue* MetalLandSoundCue;

	FTimerHandle CheckWallTimerHandle;

	void ApplyRecoil(float PitchValue, float YawValue);

	void FireDelayCompleted();
	void ReloadDelayCompleted();

	void CheckWallTick();
	FTimerHandle WalkTimerHandle;
	FTimerHandle RunTimerHandle;

	float CurrentLean;

	FVector OriginMeshVector;

	FVector M4Location;
	FRotator M4Rotation;

	FVector PistolLocation;
	FRotator PistolRotation;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> CurrentWeaponClass;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FWeaponStatsStruct CurrentStats;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EAnimStateEnum CurrentAnimState;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EItemTypeEnum CurrentWeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* CurrentReloadAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPaperSprite* WeaponIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* DT_ItemData;

public:
	Afps_cppCharacter();

	void InitializePlayerState();

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

	FTransform GetLeftHandSocketTransform() const { return bLeftHandSocketTransform; }
	void SetLeftHandSocketTransoform(FTransform LeftHandSocketTransform) { bLeftHandSocketTransform = LeftHandSocketTransform; }

	UAnimMontage* GetCurrentReloadAnimation() const { return CurrentReloadAnimation; }
	void SetCurrentReloadAnimation(UAnimMontage* _CurrentReloadAnimation) { CurrentReloadAnimation = _CurrentReloadAnimation; }

	UPaperSprite* GetWeaponIcon() const { return WeaponIcon; }
	void SetWeaponIcon(UPaperSprite* NewIcon) { WeaponIcon = NewIcon; }

	UFUNCTION(BlueprintCallable)
	Afps_cppPlayerState* GetFPSPlayerState() const { return FPSPlayerState; }

	UFUNCTION()
	void ControllerRecoil(float RecoilAmount);

	UFUNCTION()
	void ControlAim(float AimAlpha);

	UFUNCTION()
	void ResetFireRifle();

	UFUNCTION(BlueprintCallable)
	void EjectShell(FVector Location, FRotator Rotation);

	void SetWeaponLocationAndRotation(FVector NewLocation, FRotator NewRotation);

	UFUNCTION()
	void DestroyWeapon();

public:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void Landed(const FHitResult& Hit) override;

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

	void SwitchWeapon();

	void Lean(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void EquipItem(int index);

	UFUNCTION(BlueprintCallable)
	void ReceiveImpactProjectile(AActor* actor, UActorComponent* comp, FVector Loc, FVector Normal);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReceiveImpactProjectile(AActor* actor, UActorComponent* comp, FVector Loc, FVector Normal);

	UFUNCTION()
	void HandleImpactEffects(AActor* actor, FVector Loc, FVector Normal);

	UFUNCTION(Server, Reliable, WithValidation)
	void HandleImpactEffectsServer(AActor* actor, FVector Loc, FVector Normal);

	UFUNCTION(NetMulticast, Reliable)
	void HandleImpactEffectsMulticast(AActor* actor, FVector Loc, FVector Normal);

	UFUNCTION(BlueprintCallable)
	void FireProjectileToDirection();

	UFUNCTION(NetMulticast, Unreliable)
	void PlaySoundAtLocationMulticast(FVector Location, USoundBase* Sound);
	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void PlaySoundAtLocationServer(FVector Location, USoundBase* Sound);

	UFUNCTION(NetMulticast, Reliable)
	void PlaySoundWithCooldownMulticast(FVector Location, USoundBase* Sound, float Delay);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void PlaySoundWithCooldownServer(FVector Location, USoundBase* Sound, float Delay);


	UFUNCTION(NetMulticast, Unreliable)
	void SpawnEmitterAtLocationMulticast(UParticleSystem* EmitterTemplate, FVector Location, FRotator Rotation, FVector Scale);
	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void SpawnEmitterAtLocationServer(UParticleSystem* EmitterTemplate, FVector Location, FRotator Rotation, FVector Scale);

	UFUNCTION(NetMulticast, Unreliable)
	void SpawnActorToMulticast(TSubclassOf<AActor> Class, FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride);
	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void SpawnActorToServer(TSubclassOf<AActor> Class, FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride);

	UFUNCTION(NetMulticast, Reliable)
	void PlayAnimMontageMulticast(UAnimMontage* AnimMontage);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void PlayAnimMontageServer(UAnimMontage* AnimMontage);

	UFUNCTION(NetMulticast, Reliable)
	void SpawnBulletHoleMulticast(FTransform SpawnTransform);

	UFUNCTION(NetMulticast, Reliable)
	void SpawnPickupActorMulticast(FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, FDynamicInventoryItem Item, TSubclassOf<class APickUpBase> Class);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void SpawnPickupActorServer(FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, FDynamicInventoryItem Item, TSubclassOf<class APickUpBase> Class);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SprintServer(float MaxWalkSpeed);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void DeleteItemServer(AActor* DeleteItem);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ApplyDamage(AActor* DamageActor, float BaseDamage, AActor* DamageCauser);

	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void WallDistanceServer(float WallDistance);
	UFUNCTION(NetMulticast, Reliable)
	void WallDistanceMulticast(float WallDistance);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void SetLeanLeftServer(bool LeanLeft);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void SetLeanRightServer(bool LeanRight);

	UFUNCTION(Client, Reliable)
	void PlayShotSequenceClient(EItemTypeEnum WeaponType);

	UFUNCTION(NetMulticast, Reliable)
	void PlayShotSequenceMulticast(EItemTypeEnum WeaponType);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void PlayShotSequenceServer(EItemTypeEnum WeaponType);


	UFUNCTION(Client, Reliable)
	void PlayReloadSequenceClient(EItemTypeEnum WeaponType);
	UFUNCTION(NetMulticast, Reliable)
	void PlayReloadSequenceMulticast(EItemTypeEnum WeaponType);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void PlayReloadSequenceServer(EItemTypeEnum WeaponType);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetLeanLeftBooleans(bool Left);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetLeanRightBooleans(bool Right);

	UFUNCTION(Server, Reliable, WithValidation)
	void ActivateObjectServer();
	// Function to handle the deactivation on clients
	UFUNCTION(NetMulticast, Reliable)
	void ActivateObjectMulticast();

	UFUNCTION(Server, Reliable, WithValidation)
	void DeactivateObjectServer();
	// Function to handle the deactivation on clients
	UFUNCTION(NetMulticast, Reliable)
	void DeactivateObjectMulticast();

	UFUNCTION(NetMulticast, Reliable)
	void SetWeaponDataMulticast(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetWeaponDataServer(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType);

	UFUNCTION(NetMulticast, Reliable)
	void StopLeftHandIKMulticast(bool bStop);

	UFUNCTION(Server, Reliable, WithValidation)
	void StopLeftHandIKServer(bool bStop);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void IF_GetLeftHandSocketTransform_Implementation(FTransform& OutTransform) override;
	virtual void IF_GetLeftHandSocketTransform_FPS_Implementation(FTransform& OutTransform) override;
	virtual void IF_GetHandSwayFloats_Implementation(float& SideMove, float& MouseX, float& MouseY) override;
	virtual void IF_GetIsAim_Implementation(bool& Aim) override;
	virtual void IF_GetStopLeftHandIK_Implementation(bool& StopIK) override;
	virtual void IF_GetWallDistance_Implementation(float& Value) override;
	virtual void IF_AddItemToInventory_Implementation(const FDynamicInventoryItem Item, AActor* pickUp) override;
	virtual void Server_DeleteItem_Implementation(AActor* ItemToDelete) override;
	virtual bool Server_DeleteItem_Validate(AActor* ItemToDelete) override;
	virtual void IF_GetAnimState_Implementation(EAnimStateEnum& AnimState) override;
	virtual void IF_GetAimAlpha_Implementation(float& A) override;
	virtual void IF_GetLeanBooleans_Implementation(bool& Left, bool& Right) override;
	virtual void IF_ReceiveProjectileImpact_Implementation(AActor* HitActor, UActorComponent* HitComponent, const FVector HitLocation, const FVector NormalPoint) override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	void ServerReceiveImpactProjectile_Implementation(AActor* actor, UActorComponent* comp, FVector Loc, FVector Normal);
	bool ServerReceiveImpactProjectile_Validate(AActor* actor, UActorComponent* comp, FVector Loc, FVector Normal);

	void HandleImpactEffectsServer_Implementation(AActor* actor, FVector Loc, FVector Normal);
	bool HandleImpactEffectsServer_Validate(AActor* actor, FVector Loc, FVector Normal);
	void HandleImpactEffectsMulticast_Implementation(AActor* actor, FVector Loc, FVector Normal);

	void PlaySoundAtLocationMulticast_Implementation(FVector Location, USoundBase* Sound);
	void PlaySoundAtLocationServer_Implementation(FVector Location, USoundBase* Sound);

	void PlaySoundWithCooldownMulticast_Implementation(FVector Location, USoundBase* Sound, float Delay);
	void PlaySoundWithCooldownServer_Implementation(FVector Location, USoundBase* Sound, float Delay);

	void SpawnEmitterAtLocationMulticast_Implementation(UParticleSystem* EmitterTemplate, FVector Location, FRotator Rotation, FVector Scale);
	void SpawnEmitterAtLocationServer_Implementation(UParticleSystem* EmitterTemplate, FVector Location, FRotator Rotation, FVector Scale);

	void SpawnActorToMulticast_Implementation(TSubclassOf<AActor> Class, FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride);
	void SpawnActorToServer_Implementation(TSubclassOf<AActor> Class, FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride);

	void SprintServer_Implementation(float MaxWalkSpeed);
	bool SprintServer_Validate(float MaxWalkSpeed);

	void SpawnBulletHoleMulticast_Implementation(FTransform SpawnTransform);

	void PlayAnimMontageMulticast_Implementation(UAnimMontage* AnimMontage);
	void PlayAnimMontageServer_Implementation(UAnimMontage* AnimMontage);
	bool PlayAnimMontageServer_Validate(UAnimMontage* AnimMontage);

	void SpawnPickupActorMulticast_Implementation(FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, FDynamicInventoryItem Item, TSubclassOf<class APickUpBase> Class);
	void SpawnPickupActorServer_Implementation(FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, FDynamicInventoryItem Item, TSubclassOf<class APickUpBase> Class);
	bool SpawnPickupActorServer_Validate(FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, FDynamicInventoryItem Item, TSubclassOf<class APickUpBase> Class);

	void DeleteItemServer_Implementation(AActor* DeleteItem);
	bool DeleteItemServer_Validate(AActor* DeleteItem);

	void ApplyDamage_Implementation(AActor* DamageActor, float BaseDamage, AActor* DamageCauser);

	void WallDistanceMulticast_Implementation(float WallDistance);
	void WallDistanceServer_Implementation(float WallDistance);

	void SetLeanLeftServer_Implementation(bool LeanLeft);
	bool SetLeanLeftServer_Validate(bool LeanLeft);

	void SetLeanRightServer_Implementation(bool LeanRight);
	bool SetLeanRightServer_Validate(bool LeanRight);

	void Multicast_SetLeanLeftBooleans_Implementation(bool Left);
	void Multicast_SetLeanRightBooleans_Implementation(bool Right);

	void PlayShotSequenceClient_Implementation(EItemTypeEnum WeaponType);
	void PlayShotSequenceMulticast_Implementation(EItemTypeEnum WeaponType);
	void PlayShotSequenceServer_Implementation(EItemTypeEnum WeaponType);
	bool PlayShotSequenceServer_Validate(EItemTypeEnum WeaponType);

	void PlayReloadSequenceClient_Implementation(EItemTypeEnum WeaponType);
	void PlayReloadSequenceMulticast_Implementation(EItemTypeEnum WeaponType);
	void PlayReloadSequenceServer_Implementation(EItemTypeEnum WeaponType);
	bool PlayReloadSequenceServer_Validate(EItemTypeEnum WeaponType);

	void ActivateObjectServer_Implementation();
	bool ActivateObjectServer_Validate();
	void ActivateObjectMulticast_Implementation();

	void DeactivateObjectServer_Implementation();
	bool DeactivateObjectServer_Validate();
	void DeactivateObjectMulticast_Implementation();

	void StopLeftHandIKMulticast_Implementation(bool bStop);
	void StopLeftHandIKServer_Implementation(bool bStop);
	bool StopLeftHandIKServer_Validate(bool bStop);

	void SetWeaponDataMulticast_Implementation(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType);
	void SetWeaponDataServer_Implementation(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType);
	bool SetWeaponDataServer_Validate(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStopK, EItemTypeEnum WeaponType);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
};

