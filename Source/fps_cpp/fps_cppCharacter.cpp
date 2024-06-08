// Copyright Epic Games, Inc. All Rights Reserved.

#include "fps_cppCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Engine/DataTable.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "Math/RotationMatrix.h"
#include "Math/Transform.h"
#include "Kismet/GameplayStatics.h"
#include "BulletHole.h"
#include "Sound/SoundCue.h"
#include "PlayerInterfaceImplement.h"
#include "Particles/ParticleSystem.h"
#include "BloodDecal.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon_Base_Pistol.h"
#include "Weapon_Base_M4.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// Afps_cppCharacter

Afps_cppCharacter::Afps_cppCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh(), FName("neck_02"));
	CameraBoom->TargetArmLength = 0.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

	WeaponBase = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponBase"));
	WeaponBase->SetupAttachment(GetMesh(), FName(TEXT("WeaponSocket")));
	WeaponBase->SetChildActorClass(AWeapon_Base::StaticClass());

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	bReplicates = true;
	bAlwaysRelevant = true;

	bSprinting = false;
	bSoundPlaying = false;
	bIsAiming = false;
	bStopLeftHandIK = false;
	bIsAttacking = false;

	GetCharacterMovement()->MaxWalkSpeed = 100.0f;

	InventoryComponent = CreateDefaultSubobject<UInventory>(TEXT("InventoryComponent"));

	bAnimState = EAnimStateEnum::Hands;
	bCurrentItemSelection = 0;

	bHealth = 200.0f;

	Tags.Add(FName("Flesh"));
	Tags.Add(FName("Player"));

	static ConstructorHelpers::FObjectFinder<UCurveFloat> RecoilCurve(TEXT("/Game/Characters/Mannequins/Animations/RecoilTrack"));
	if (RecoilCurve.Succeeded()) 
	{
		bRecoilCurve = RecoilCurve.Object;
	}

	static ConstructorHelpers::FObjectFinder<UCurveFloat> AimCurve(TEXT("/Game/Characters/Mannequins/Animations/AimTrack"));
	if (AimCurve.Succeeded())
	{
		bAimCurve = AimCurve.Object;
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> BlueprintAnim(TEXT("/Game/Characters/Mannequins/Animations/ABP_Manny"));
	if (BlueprintAnim.Succeeded())
	{
		bAnimationBlueprintRef = BlueprintAnim.Class;
		GetMesh()->SetAnimInstanceClass(bAnimationBlueprintRef);
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> SoundCueFinder(TEXT("/Game/MilitaryWeapDark/Sound/Rifle/RifleB_FireEnd_Cue.RifleB_FireEnd_Cue"));
	if (SoundCueFinder.Succeeded())
	{
		RifleImpactSoundCue = SoundCueFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> RifleSurfaceSoundFinder(TEXT("/Game/MilitaryWeapDark/Sound/Rifle/Rifle_ImpactSurface_Cue.Rifle_ImpactSurface_Cue"));
	if (RifleSurfaceSoundFinder.Succeeded())
	{
		RifleSurfaceImpactSoundCue = RifleSurfaceSoundFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> PistolurfaceSoundFinder(TEXT("/Game/MilitaryWeapSilver/Sound/Pistol/Cues/PistolA_Fire_Cue"));
	if (PistolurfaceSoundFinder.Succeeded())
	{
		PistolSurfaceImpactSoundCue = PistolurfaceSoundFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> EmitterTemplateFinder(TEXT("/Game/MilitaryWeapDark/FX/P_AssaultRifle_MuzzleFlash"));
	if (EmitterTemplateFinder.Succeeded())
	{
		MuzzleFlashParticleSystem = EmitterTemplateFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> MetalImpactFinder(TEXT("/Game/MilitaryWeapSilver/FX/P_Impact_Metal_Large_01"));
	if (MetalImpactFinder.Succeeded())
	{
		MetalImpactParticleSystem = MetalImpactFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> StoneImpactFinder(TEXT("/Game/MilitaryWeapDark/FX/P_Impact_Stone_Large_01"));
	if (StoneImpactFinder.Succeeded())
	{
		StoneImpactParticleSystem = StoneImpactFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> Finder1(TEXT("/Game/FootStepSound/Footsteps_Metal/Footsteps_Metal_Walk/Metal_Walk_Cue_1"));
	if (Finder1.Succeeded())
	{
		MetalWalkSoundCue = Finder1.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundCue> Finder2(TEXT("/Game/FootStepSound/Footsteps_Metal/Footsteps_Metal_Run/Metal_Run_Cue_1"));
	if (Finder2.Succeeded())
	{
		MetalRunSoundCue = Finder2.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundCue> Finder3(TEXT("/Game/FootStepSound/Footsteps_Metal/Footsteps_Metal_Jump/Metal_JumpStart_Cue_1"));
	if (Finder3.Succeeded())
	{
		MetalJumpSoundCue = Finder3.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundCue> Finder4(TEXT("/Game/FootStepSound/Footsteps_Metal/Footsteps_Metal_Jump/Metal_Land_Cue_1"));
	if (Finder4.Succeeded())
	{
		MetalLandSoundCue = Finder4.Object;
	}
}

void Afps_cppCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void Afps_cppCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 소유권 설정
	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		SetOwner(PC);

		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController)
		{
			UE_LOG(LogTemp, Log, TEXT("BeginPlay: Character controlled by PlayerController"));
			SetOwner(PlayerController);

			// Initialize the PlayerInterface
			PlayerInterface.SetInterface(Cast<IPlayerInterface>(this));
			PlayerInterface.SetObject(this);

			if (!PlayerInterface.GetObject() || !PlayerInterface.GetInterface())
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to initialize PlayerInterface"));
			}
		}
	}
}

void Afps_cppCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UE_LOG(LogTemp, Log, TEXT("BeginPlay: Character controlled by PlayerController"));
		SetOwner(PlayerController);

		// Initialize the PlayerInterface
		PlayerInterface.SetInterface(Cast<IPlayerInterface>(this));
		PlayerInterface.SetObject(this);
	}

	if (bRecoilCurve)
	{
		FOnTimelineFloat RecoilProgressFunction;
		RecoilProgressFunction.BindUFunction(this, FName("ControllerRecoil"));

		bRecoilTimeline.AddInterpFloat(bRecoilCurve, RecoilProgressFunction);
	}

	if (bAimCurve)
	{
		FOnTimelineFloat AimProgressFunction;
		AimProgressFunction.BindUFunction(this, FName("ControlAim"));

		bAimTimeline.AddInterpFloat(bAimCurve, AimProgressFunction); 
		bAimTimeline.SetLooping(false);
		bAimTimeline.SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

		UE_LOG(LogTemp, Log, TEXT("AimTimeline initialized with curve: %s"), *bAimCurve->GetName());
		UFunction* Function = FindFunction(AimProgressFunction.GetFunctionName());
		if (Function)
		{
			UE_LOG(LogTemp, Log, TEXT("Function found."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Function not found."));
		}
	}

	if (bAnimationBlueprintRef)
	{
		GetMesh()->SetAnimInstanceClass(bAnimationBlueprintRef);
	}

	if (FollowCamera)
	{
		GetWorldTimerManager().SetTimer(CheckWallTimerHandle, this, &Afps_cppCharacter::CheckWallTick, 0.1f, true);
	}
	EquipItem();
}

void Afps_cppCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	GetWorldTimerManager().SetTimer(bTimerHandle_CheckWallTick, this, &Afps_cppCharacter::CheckWallTick, 0.02f, true);

	bRecoilTimeline.TickTimeline(DeltaTime);

	bAimTimeline.TickTimeline(DeltaTime);

	Lean();

	if (!FMath::IsNearlyZero(GetVelocity().Size()))
	{
		if (!bJumping)
		{
			if (GetCharacterMovement()->MaxWalkSpeed < 600.0f)
			{
				GetWorld()->GetTimerManager().ClearTimer(RunTimerHandle);
				if (HasAuthority())
				{
					PlaySoundAtLocationMulticast(GetActorLocation(), MetalWalkSoundCue, 1.0f);
				}
				else
				{
					PlaySoundAtLocationServer(GetActorLocation(), MetalWalkSoundCue, 1.0f);
				}
			}
			else
			{
				GetWorld()->GetTimerManager().ClearTimer(WalkTimerHandle);
				if (HasAuthority())
				{
					PlaySoundAtLocationMulticast(GetActorLocation(), MetalRunSoundCue, 0.3f);
				}
				else
				{
					PlaySoundAtLocationServer(GetActorLocation(), MetalRunSoundCue, 0.3f);
				}
			}
		}			
	}
	else
	{
		bWalking = false;
		bSprinting = false;
		bSoundPlaying = false;
		GetWorld()->GetTimerManager().ClearTimer(WalkTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(RunTimerHandle);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void Afps_cppCharacter::CheckWallTick()
{
	if (!FollowCamera)
	{
		return;
	}

	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 200.0f;
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("HitTrace")), true, this);
	TraceParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		TraceParams
	);

	if (bHit)
	{
		float WallDistance = FVector::Distance(HitResult.Location, FollowCamera->GetComponentLocation()) / 200.0f;
		if (HasAuthority())
		{
			WallDistanceMulticast(WallDistance);
		}
		else
		{
			WallDistanceServer(WallDistance);
		}
	}
	else
	{
		if (HasAuthority())
		{
			WallDistanceMulticast(1.0f);
		}
		else
		{
			WallDistanceServer(1.0f);
		}
	}
}

void Afps_cppCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Add Input Mapping Context
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController != nullptr)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem != nullptr)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent != nullptr) {
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &Afps_cppCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &Afps_cppCharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &Afps_cppCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &Afps_cppCharacter::Look);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &Afps_cppCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &Afps_cppCharacter::StopSprint);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &Afps_cppCharacter::Fire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &Afps_cppCharacter::StopFire);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &Afps_cppCharacter::Aiming);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &Afps_cppCharacter::StopAiming);

		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &Afps_cppCharacter::Reload);

		EnhancedInputComponent->BindAction(DropItemAction, ETriggerEvent::Started, this, &Afps_cppCharacter::DropItem);

		EnhancedInputComponent->BindAction(SwitchAction, ETriggerEvent::Triggered, this, &Afps_cppCharacter::SwitchWeapon);

		EnhancedInputComponent->BindAction(LeanAction, ETriggerEvent::Triggered, this, &Afps_cppCharacter::Lean);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void Afps_cppCharacter::Jump()
{
	Super::Jump();
	bJumping = true;

	if (HasAuthority())
	{
		PlaySoundAtLocationMulticast(GetActorLocation(), MetalJumpSoundCue, 0.0f);
	}
	else
	{
		PlaySoundAtLocationServer(GetActorLocation(), MetalJumpSoundCue, 0.0f);
	}
}
void Afps_cppCharacter::StopJumping()
{
	Super::StopJumping();
}

void Afps_cppCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	bJumping = false;
	if (HasAuthority())
	{
		PlaySoundAtLocationMulticast(GetActorLocation(), MetalLandSoundCue, 0.0f);
	}
	else
	{
		PlaySoundAtLocationServer(GetActorLocation(), MetalLandSoundCue, 0.0f);
	}
}

void Afps_cppCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void Afps_cppCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void Afps_cppCharacter::Sprint()
{
	bSprinting = true;

	if (HasAuthority())
	{
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		}
	}
	else
	{
		SprintServer(600.0f);
	}
}

void Afps_cppCharacter::StopSprint()
{
	bSprinting = false;

	if (HasAuthority())
	{
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = 100.0f;
		}
	}
	else
	{
		SprintServer(100.0f);
	}
}

void Afps_cppCharacter::ApplyRecoil(float PitchValue, float YawValue)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	float SelectFloat_1 = bIsAiming ? -0.25f : -1.0f;
	float SelectFloat_2 = bIsAiming ? 1.0f : 2.5f;
	if (PlayerController)
	{
		PlayerController->AddPitchInput(SelectFloat_1 * PitchValue);
		if (FMath::RandRange(int32(1), int32(10)) < 5)
		{
			PlayerController->AddYawInput((YawValue / SelectFloat_2) * -1);
		}
		else
		{
			PlayerController->AddYawInput(YawValue / SelectFloat_2);
		}
	}
}

void Afps_cppCharacter::ControllerRecoil(float Value)
{
	float CurveValue = bRecoilCurve->GetFloatValue(Value);
	float PitchValue = FMath::Lerp(0.0f, bCurrentStats.InputRecoil, CurveValue);
	float YawValue = FMath::Lerp(0.0f, bCurrentStats.InputRecoil, CurveValue);

	ApplyRecoil(PitchValue, YawValue);
}

void Afps_cppCharacter::ControlAim(float Value)
{
	UE_LOG(LogTemp, Log, TEXT("ControlAim called with Value: %f"), Value);

	if (CameraBoom && CameraBoom->IsValidLowLevel())
	{
		FVector NewLocation = FMath::Lerp(FVector(10.385671f, 1.909163f, -1.909163f), FVector(13.945591f, 1.909163f, 12.256057f), Value);
		CameraBoom->SetRelativeLocation(NewLocation);

		UE_LOG(LogTemp, Log, TEXT("CameraBoom location set to: %s"), *NewLocation.ToString());
	}
}

void Afps_cppCharacter::ResetFireRifle()
{
	bFireCooldownTimer.Invalidate();
}

void Afps_cppCharacter::EjectShell(FVector Location, FRotator Rotation)
{
	if (Location != FVector(0, 0, 0))
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(FQuat(Rotation));
		SpawnTransform.SetScale3D(FVector(1, 1, 1));

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = nullptr;

		AShell_Base* ShellActor = GetWorld()->SpawnActor<AShell_Base>(AShell_Base::StaticClass(), SpawnTransform, SpawnParams);
		if (ShellActor)
		{
			ShellActor->GetShell()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			ShellActor->GetShell()->SetSimulatePhysics(true);
			FVector RightRandom = ShellActor->GetShell()->GetRightVector() * FMath::RandRange(int32(-2), int32(2));
			FVector ForwardRandom = ShellActor->GetShell()->GetForwardVector() * FMath::RandRange(int32(1), int32(10));
			ShellActor->GetShell()->AddImpulse(RightRandom + ForwardRandom);

			GetWorldTimerManager().SetTimer(bShellEjectTimer, nullptr, 0.5f, false);
		}
	}
}

void Afps_cppCharacter::Fire()
{
	bIsAttacking = true;

	switch (bWeaponType)
	{
	case EItemTypeEnum::Pistol:
		PistolFire();
		break;

	case EItemTypeEnum::Rifle:
		RifleFire();
		break;

	default:
		break;
	}
}

void Afps_cppCharacter::RifleFire()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(bFireCooldownTimer))
	{
		if (bIsAttacking)
		{
			if (InventoryComponent->GetCurBullet(bCurrentItemSelection) >= 1)
			{
				GetWorld()->GetTimerManager().SetTimer(
					bFireCooldownTimer,
					this,
					&Afps_cppCharacter::ResetFireRifle,
					bCurrentStats.FireRate,
					false
				);
				InventoryComponent->ReduceBullet(bCurrentItemSelection);
				if (WeaponBase && WeaponBase->GetChildActor())
				{
					AWeapon_Base_M4* Weapon = Cast<AWeapon_Base_M4>(WeaponBase->GetChildActor());
					if (Weapon)
					{
						if (!Weapon->GetIsReplicated())
						{
							Weapon->SetReplicates(true);
						}
						FTransform ShellTransform;
						IGunInterface::Execute_GetShellTransform(Weapon, ShellTransform);
						EjectShell(ShellTransform.GetLocation(), FRotator(ShellTransform.GetRotation()));
						FireProjectileToDirection();

						if (GetMesh()->GetAnimInstance())
						{
							UFunction* F_Procedural_Recoil = GetMesh()->GetAnimInstance()->FindFunction(TEXT("f_ProceduralRecoil"));
							if (F_Procedural_Recoil)
							{
								GetMesh()->GetAnimInstance()->ProcessEvent(F_Procedural_Recoil, &bCurrentStats.ProceduralRecoil);
								if (HasAuthority())
								{
									PlayShotSequenceMulticast(EItemTypeEnum::Rifle);
								}
								else
								{
									PlayShotSequenceServer(EItemTypeEnum::Rifle);
								}
								GetWorld()->GetTimerManager().SetTimer(bFireRateTimer, this, &Afps_cppCharacter::FireDelayCompleted, bCurrentStats.FireRate, false);
							}
						}

					}
				}
			}
		}
	}
}

void Afps_cppCharacter::PistolFire()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(bFireCooldownTimer))
	{
		if (bIsAttacking)
		{
			if (InventoryComponent->GetCurBullet(bCurrentItemSelection) >= 1)
			{
				GetWorld()->GetTimerManager().SetTimer(
					bFireCooldownTimer,
					this,
					&Afps_cppCharacter::ResetFireRifle,
					bCurrentStats.FireRate,
					false
				);
				InventoryComponent->ReduceBullet(bCurrentItemSelection);
				if (WeaponBase && WeaponBase->GetChildActor())
				{
					AWeapon_Base_Pistol* Weapon = Cast<AWeapon_Base_Pistol>(WeaponBase->GetChildActor());
					if (Weapon)
					{
						FTransform ShellTransform;
						IGunInterface::Execute_GetShellTransform(Weapon, ShellTransform);
						EjectShell(ShellTransform.GetLocation(), FRotator(ShellTransform.GetRotation()));
						FireProjectileToDirection();

						if (GetMesh()->GetAnimInstance())
						{
							UFunction* F_Procedural_Recoil = GetMesh()->GetAnimInstance()->FindFunction(TEXT("f_ProceduralRecoil"));
							if (F_Procedural_Recoil)
							{
								GetMesh()->GetAnimInstance()->ProcessEvent(F_Procedural_Recoil, &bCurrentStats.ProceduralRecoil);
								if (HasAuthority())
								{
									PlayShotSequenceMulticast(EItemTypeEnum::Pistol);
								}
								else
								{
									PlayShotSequenceServer(EItemTypeEnum::Pistol);
								}
								GetWorld()->GetTimerManager().SetTimer(bFireRateTimer, this, &Afps_cppCharacter::FireDelayCompleted, bCurrentStats.FireRate, false);
							}
						}

					}
				}
			}
		}
	}
}


void Afps_cppCharacter::StopFire()
{
	bIsAttacking = false;
}

void Afps_cppCharacter::Aiming()
{
	bIsAiming = true;
	bAimTimeline.Play();
}

void Afps_cppCharacter::StopAiming()
{
	bIsAiming = false;
	bAimTimeline.Reverse();
}

void Afps_cppCharacter::FireDelayCompleted()
{
	if (bIsAttacking && InventoryComponent->GetInventory()[bCurrentItemSelection].Bullets >= 1)
	{
		RifleFire();
	}
	
}

void Afps_cppCharacter::ReloadDelayCompleted()
{
	bIsAiming = true;
}

void Afps_cppCharacter::Reload()
{
	if (!InventoryComponent) {
		UE_LOG(LogTemp, Error, TEXT("Inventory is nullptr"));
		return;
	}
	if (InventoryComponent->GetInventory()[bCurrentItemSelection].Bullets < bCurrentStats.MagSize)
	{
		bIsAiming = false;
		bIsAttacking = false;
		if (HasAuthority())
		{
			StopLeftHandIKMulticast(true);
		}
		else
		{
			StopLeftHandIKServer(true);
		}
		if (WeaponBase && WeaponBase->GetChildActor())
		{
			AWeapon_Base* WB = Cast<AWeapon_Base>(WeaponBase->GetChildActor());
			if (bCurrentReloadAnimation)
			{
				if (HasAuthority())
				{
					PlayAnimMontageMulticast(bCurrentReloadAnimation);
				}
				else
				{
					PlayAnimMontageServer(bCurrentReloadAnimation);
				}

				AWeapon_Base_M4* WB_M4 = Cast<AWeapon_Base_M4>(WB);
				if (WB_M4)
				{
					if (HasAuthority())
					{
						PlayReloadSequenceMulticast(EItemTypeEnum::Rifle);
					}
					else
					{
						PlayReloadSequenceServer(EItemTypeEnum::Rifle);
					}
				}

				AWeapon_Base_Pistol* WB_Pistol = Cast<AWeapon_Base_Pistol>(WB);
				if (WB_Pistol)
				{
					if (HasAuthority())
					{
						PlayReloadSequenceMulticast(EItemTypeEnum::Pistol);
					}
					else
					{
						PlayReloadSequenceServer(EItemTypeEnum::Pistol);
					}
				}

				GetWorld()->GetTimerManager().SetTimer(
					bFireCooldownTimer,
					this,
					&Afps_cppCharacter::ReloadDelayCompleted,
					bCurrentStats.ReloadTime,
					false
				);

				if (InventoryComponent && InventoryComponent->GetInventory().IsValidIndex(bCurrentItemSelection))
				{
					InventoryComponent->ReloadBullet(bCurrentItemSelection, bCurrentStats);
					if (HasAuthority())
					{
						StopLeftHandIKMulticast(false);
					}
					else
					{
						StopLeftHandIKServer(false);
					}
				}
			}
			
		}	
	}
}

void Afps_cppCharacter::DropItem()
{
	if (!InventoryComponent) {
		UE_LOG(LogTemp, Error, TEXT("Inventory is nullptr"));
		return;
	}
	if (InventoryComponent->GetInventory()[bCurrentItemSelection].ID > 0)
	{
		if (InventoryComponent->GetInventory().IsValidIndex(bCurrentItemSelection))
		{
			FDynamicInventoryItem Item = InventoryComponent->GetInventory()[bCurrentItemSelection];
			FVector ForwardCameraLocation = FollowCamera->GetComponentLocation() + (FollowCamera->GetForwardVector() * 200.0);

			FTransform SpawnTransform;
			SpawnTransform.SetLocation(ForwardCameraLocation);
			SpawnTransform.SetRotation(FQuat(FRotator(0, 0, 0)));
			SpawnTransform.SetScale3D(FVector(1, 1, 1));
			if (HasAuthority())
			{
				SpawnPickupActorMulticast(SpawnTransform, ESpawnActorCollisionHandlingMethod::Undefined, Item, bCurrentWeaponPickUpClass);
			}
			else
			{
				SpawnPickupActorServer(SpawnTransform, ESpawnActorCollisionHandlingMethod::Undefined, Item, bCurrentWeaponPickUpClass);
			}
			InventoryComponent->GetInventory().RemoveAt(bCurrentItemSelection);
			bCurrentItemSelection = 0;
			EquipItem();
		}
	}
}

void Afps_cppCharacter::SetWeaponLocationAndRotation(FVector NewLocation, FRotator NewRotation)
{
	if (WeaponBase && WeaponBase->GetChildActor())
	{
		AActor* WeaponChildActor = WeaponBase->GetChildActor();
		if (WeaponChildActor)
		{
			WeaponChildActor->SetActorRelativeLocation(NewLocation);

			// Log the applied rotation values
			FRotator AppliedRotation = WeaponChildActor->GetActorRotation();
			UE_LOG(LogTemp, Warning, TEXT("SetWeaponLocationAndRotation - Applied Rotation - Pitch: %f, Yaw: %f, Roll: %f"), AppliedRotation.Pitch, AppliedRotation.Yaw, AppliedRotation.Roll);
		}
	}
}

void Afps_cppCharacter::SwitchWeapon()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (PlayerController->IsInputKeyDown(EKeys::One))
		{
			bCurrentItemSelection = 0;
			EquipItem();
		}
		else if (PlayerController->IsInputKeyDown(EKeys::Two))
		{
			bCurrentItemSelection = 1;
			EquipItem();
		}
		else if (PlayerController->IsInputKeyDown(EKeys::Three))
		{
			bCurrentItemSelection = 2;
			EquipItem();
		}
	}
}

void Afps_cppCharacter::Lean()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (PlayerController->IsInputKeyDown(EKeys::Q))
		{
			if (HasAuthority())
			{
				bLeanLeft = true;
			}
			else
			{
				SetLeanLeftServer(true);
			}
		}
		else
		{
			if (HasAuthority())
			{
				bLeanLeft = false;
			}
			else
			{
				SetLeanLeftServer(false);
			}
		}

		if (PlayerController->IsInputKeyDown(EKeys::E))
		{
			if (HasAuthority())
			{
				bLeanRight = true;
			}
			else
			{
				SetLeanRightServer(true);
			}
		}
		else
		{
			if (HasAuthority())
			{
				bLeanRight = false;
			}
			else
			{
				SetLeanRightServer(false);
			}
		}
	}
}

void Afps_cppCharacter::EquipItem()
{
	if (IsLocallyControlled()) {
		if (!InventoryComponent) {
			UE_LOG(LogTemp, Error, TEXT("Inventory is nullptr"));
			return;
		}

		if (!PlayerInterface) {
			UE_LOG(LogTemp, Error, TEXT("PlayerInterface is null"));
			return;
		}

		int CurrentSelection = bCurrentItemSelection;
		if (!InventoryComponent->GetInventory().IsValidIndex(CurrentSelection)) {
			UE_LOG(LogTemp, Error, TEXT("Invalid inventory index: %d"), CurrentSelection);
			return;
		}

		int id = InventoryComponent->GetInventory()[CurrentSelection].ID;
		FString fname = FString::FromInt(id);
		DT_ItemData = LoadObject<UDataTable>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/inventory/ItemData/DT_ItemData"));
		if (DT_ItemData) {
			TArray<FName> RowNames = DT_ItemData->GetRowNames();
			for (int i = 0; i < RowNames.Num(); i++) {
				if (RowNames[i] == fname)
				{
					FItemDataTable* data = DT_ItemData->FindRow<FItemDataTable>(RowNames[i], RowNames[i].ToString());
					if (data)
					{
						if (HasAuthority())
						{
							SetWeaponClassMulticast(data->WeaponClass);
							SetStatsToMulticast(data->Stats);
							SetAnimStateMulticast(data->AnimState);
						}
						else
						{
							SetWeaponClassServer(data->WeaponClass);
							SetStatsToServer(data->Stats);
							SetAnimStateServer(data->AnimState);
						}
						SetWeaponIcon(data->icon);
						SetCurrentStats(data->Stats);
						SetCurrentReloadAnimation(data->ReloadAnimation);
						SetCurrentWeaponClass(data->WeaponClass);
						SetWeaponType(data->Type);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Failed to find data for item %s"), *fname);
					}
					break;
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load item data table"));
		}
	}
}

void Afps_cppCharacter::FireProjectileToDirection()
{
	FVector MuzzlePointLocalLocation;
	FTransform MuzzlePoint = WeaponBase->GetSocketTransform(FName("MuzzlePoint"));
	MuzzlePointLocalLocation = MuzzlePoint.GetLocation();

	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * bCurrentStats.Range;
	FHitResult HitResult;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility
	);

	if (bHit) 
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;
		SpawnParams.Instigator = nullptr;

		FTransform SpawnTransform;

		FRotator Selected;
		FRotator RotatorA = FRotationMatrix::MakeFromX(HitResult.ImpactPoint - MuzzlePointLocalLocation).Rotator();
		FRotator RotatorB = FRotationMatrix::MakeFromX(HitResult.ImpactPoint - MuzzlePointLocalLocation).Rotator();
		RotatorB.Roll += FMath::RandRange(-1.0f, 1.0f);
		RotatorB.Pitch += FMath::RandRange(-1.0f, 1.0f);
		RotatorB.Yaw += FMath::RandRange(-1.0f, 1.0f);

		if (bIsAiming)
		{
			Selected = RotatorA;
		}
		else
		{
			Selected = RotatorB;
		}

		SpawnTransform = FTransform(Selected, MuzzlePointLocalLocation, FVector(1, 1, 1));

		AProjectileBullet* SpawnedBulletActor = GetWorld()->SpawnActor<AProjectileBullet>(AProjectileBullet::StaticClass(), SpawnTransform, SpawnParams);
		SpawnedBulletActor->SetPlayer(this);
		SpawnedBulletActor->GetProjectileMovment()->Velocity *= 1.0f;
	}
	else
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;
		SpawnParams.Instigator = nullptr;

		FTransform SpawnTransform;

		FTransform tmpTransform = FTransform(FollowCamera->GetComponentRotation(), MuzzlePointLocalLocation, FVector(1, 1, 1));
		FRotator RotatorA = FRotator(tmpTransform.GetRotation());
		FRotator RotatorB = FRotator(tmpTransform.GetRotation());
		FRotator Selected;
		RotatorB.Roll += FMath::RandRange(-1.0f, 1.0f);
		RotatorB.Pitch += FMath::RandRange(-1.0f, 1.0f);
		RotatorB.Yaw += FMath::RandRange(-1.0f, 1.0f);
		
		if (bIsAiming)
		{
			Selected = RotatorA;
		}
		else
		{
			Selected = RotatorB;
		}

		SpawnTransform = FTransform(Selected, tmpTransform.GetLocation(), tmpTransform.GetScale3D());
		AProjectileBullet* SpawnedBulletActor = GetWorld()->SpawnActor<AProjectileBullet>(AProjectileBullet::StaticClass(), SpawnTransform, SpawnParams);
		SpawnedBulletActor->SetPlayer(this);
		SpawnedBulletActor->GetProjectileMovment()->Velocity *= 10000.0f;;
	}
}

void Afps_cppCharacter::ReceiveImpactProjectile(AActor* actor, UActorComponent* comp, FVector Loc, FVector Normal)
{
	FTransform tmpTransform = FTransform(FRotationMatrix::MakeFromX(Normal).Rotator(), Loc, FVector(1, 1, 1));
	if (actor)
	{
		ApplyDamageServer(actor, bCurrentStats.Damage, this);

		// 서버에서만 호출
		if (HasAuthority())
		{
			SpawnBulletHoleMulticast(tmpTransform);
		}
		else
		{
			SpawnBulletHoleServer(tmpTransform);
		}
		USoundCue* WeaponSound = nullptr;
		if (bCurrentWeaponClass == AWeapon_Base_M4::StaticClass())
		{
			WeaponSound = RifleSurfaceImpactSoundCue;
		}
		else if (bCurrentWeaponClass == AWeapon_Base_Pistol::StaticClass())
		{
			WeaponSound = PistolSurfaceImpactSoundCue;
		}
		if (actor->ActorHasTag("Metal"))
		{
			if (HasAuthority())
			{
				PlaySoundAtLocationMulticast(FollowCamera->GetComponentLocation(), WeaponSound, 0.0f);
				SpawnEmitterAtLocationMulticast(MetalImpactParticleSystem, Loc, FRotator(0, 0, 0), FVector(1, 1, 1));
			}
			else
			{
				PlaySoundAtLocationServer(FollowCamera->GetComponentLocation(), WeaponSound, 0.0f);
				SpawnEmitterAtLocationServer(MetalImpactParticleSystem, Loc, FRotator(0, 0, 0), FVector(1, 1, 1));
			}
		}
		else if (actor->ActorHasTag("Stone"))
		{
			if (HasAuthority())
			{
				PlaySoundAtLocationMulticast(FollowCamera->GetComponentLocation(), WeaponSound, 0.0f);
				SpawnEmitterAtLocationMulticast(StoneImpactParticleSystem, Loc, FRotator(0, 0, 0), FVector(1, 1, 1));
			}
			else
			{
				PlaySoundAtLocationServer(FollowCamera->GetComponentLocation(), WeaponSound, 0.0f);
				SpawnEmitterAtLocationServer(StoneImpactParticleSystem, Loc, FRotator(0, 0, 0), FVector(1, 1, 1));
			}
		}
		else if (actor->ActorHasTag("Player"))
		{
			if (HasAuthority())
			{
				PlaySoundAtLocationMulticast(FollowCamera->GetComponentLocation(), WeaponSound, 0.0f);
				SpawnEmitterAtLocationMulticast(MetalImpactParticleSystem, Loc, FRotator(0, 0, 0), FVector(1, 1, 1));
			}
			else
			{
				PlaySoundAtLocationServer(FollowCamera->GetComponentLocation(), WeaponSound, 0.0f);
				SpawnEmitterAtLocationServer(MetalImpactParticleSystem, Loc, FRotator(0, 0, 0), FVector(1, 1, 1));
			}

			FVector Start = Loc;
			FVector End = FollowCamera->GetForwardVector() * 1500.0f + Loc;
			FCollisionQueryParams TraceParams(FName(TEXT("HitTrace")), true, this);
			TraceParams.AddIgnoredActor(actor);  // Ignore the original hit actor
			FHitResult HitResult;

			bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_Visibility,
				TraceParams
			);

			if (bHit)
			{
				FTransform tmpTransform2 = FTransform(FRotationMatrix::MakeFromX(HitResult.Normal).Rotator(), HitResult.Location, FVector(1, 1, 1));
				TSubclassOf<AActor> Blood = ABloodDecal::StaticClass();
				if (Blood)
				{
					if (HasAuthority())
					{
						SpawnActorToMulticast(Blood, tmpTransform2, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
					}
					else
					{
						SpawnActorToServer(Blood, tmpTransform2, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
					}
				}
			}

		}
	}
}

void Afps_cppCharacter::DeleteItemServer_Implementation(AActor* DeleteItem)
{
	if (GetLocalRole() < ROLE_Authority) {
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

		if (IPlayerInterface* playerInterface = Cast<IPlayerInterface>(PlayerController->GetCharacter())) {
			playerInterface->Server_DeleteItem(DeleteItem);
		}
	}
	else {
		if (DeleteItem && DeleteItem->IsValidLowLevel())
		{
			DeleteItem->Destroy();
		}
	}
}

bool Afps_cppCharacter::DeleteItemServer_Validate(AActor* DeleteItem)
{
	return true;
}

void Afps_cppCharacter::SprintServer_Implementation(float MaxWalkSpeed)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	}
}
bool Afps_cppCharacter::SprintServer_Validate(float MaxWalkSpeed) 
{
	return true;
}

void Afps_cppCharacter::PlaySoundAtLocationMulticast_Implementation(FVector Location, USoundBase* Sound, float Delay)
{
	if (GetWorld())
	{
		if (!FMath::IsNearlyZero(Delay))
		{
			PlaySoundWithCooldown(Location, Sound, Delay);
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(this, Sound, Location);
		}
	}
}

void Afps_cppCharacter::PlaySoundWithCooldown(FVector Location, USoundBase* Sound, float Delay)
{
	FTimerHandle& TimerHandle = (GetCharacterMovement()->MaxWalkSpeed < 600.0f) ? WalkTimerHandle : RunTimerHandle;

	// TimerHandle 초기화
	if (!TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, &TimerHandle]()
			{
				// 타이머가 만료되었을 때 사운드 재생 상태를 false로 설정
				if (TimerHandle.IsValid())
				{
					bSoundPlaying = false;
				}
			}), Delay, true);

		// 사운드 재생
	}
	if (!bSoundPlaying)
	{
		bSoundPlaying = true;
		UGameplayStatics::PlaySoundAtLocation(this, Sound, Location);
	}
}

void Afps_cppCharacter::PlaySoundAtLocationServer_Implementation(FVector Location, USoundBase* Sound, float Delay)
{
	if (HasAuthority()) {
		PlaySoundAtLocationMulticast(Location, Sound, Delay);
	}
	
}

void Afps_cppCharacter::SpawnEmitterAtLocationMulticast_Implementation(UParticleSystem* EmitterTemplate, FVector Location, FRotator Rotation, FVector Scale)
{
	if (GetWorld())
	{
		FTransform Transform = FTransform(Rotation, Location, Scale);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EmitterTemplate, Transform);
	}
}

void Afps_cppCharacter::SpawnEmitterAtLocationServer_Implementation(UParticleSystem* EmitterTemplate, FVector Location, FRotator Rotation, FVector Scale)
{
	if (HasAuthority()) {
		SpawnEmitterAtLocationMulticast(EmitterTemplate, Location, Rotation, Scale);
	}
	
}

void Afps_cppCharacter::SetWeaponClass(TSubclassOf<AActor> WBase)
{
	if (HasAuthority())
	{
		// 서버 권한이 있는 경우 직접 멀티캐스트 함수를 호출합니다.
		SetWeaponClassMulticast(WBase);
	}
	else
	{
		// 클라이언트가 서버에 요청을 보냅니다.
		SetWeaponClassServer(WBase);
	}
}

void Afps_cppCharacter::SpawnActorToMulticast_Implementation(TSubclassOf<AActor> Class, FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride)
{
	UWorld* World = GetWorld();
	if (World && Class)
	{
		// Actor를 Spawn합니다.
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = CollisionHandlingOverride;
		SpawnParams.Instigator = GetInstigator();

		AActor* SpawnedActor = World->SpawnActor<AActor>(Class, SpawnTransform, SpawnParams);
	}
}
void Afps_cppCharacter::SpawnActorToServer_Implementation(TSubclassOf<AActor> Class, FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride)
{
	if (HasAuthority()) {
		SpawnActorToMulticast(Class, SpawnTransform, CollisionHandlingOverride);
	}
}

void Afps_cppCharacter::SetWeaponClassMulticast_Implementation(TSubclassOf<AActor> WBase)
{
	if (WeaponBase && WBase)
	{
		WeaponBase->SetChildActorClass(WBase);
	}
}

void Afps_cppCharacter::SetWeaponClassServer_Implementation(TSubclassOf<AActor> WBase)
{
	if (HasAuthority()) {
		SetWeaponClassMulticast(WBase);
	}
	
}

bool Afps_cppCharacter::SetWeaponClassServer_Validate(TSubclassOf<AActor> WBase)
{
	return true;
}

void Afps_cppCharacter::StopLeftHandIKMulticast_Implementation(bool StopLeftHandIK)
{
	bStopLeftHandIK = StopLeftHandIK;
}


void Afps_cppCharacter::StopLeftHandIKServer_Implementation(bool StopLeftHandIK)
{
	if (HasAuthority()) {
		StopLeftHandIKMulticast(StopLeftHandIK);
	}
	
}


void Afps_cppCharacter::OnRep_AnimState() {
	OnAnimStateChanged.Broadcast();
}

void Afps_cppCharacter::SetAnimStateMulticast_Implementation(EAnimStateEnum AnimState)
{
	bAnimState = AnimState;
	OnRep_AnimState();
}

void Afps_cppCharacter::SetAnimStateServer_Implementation(EAnimStateEnum AnimState)
{
	if (HasAuthority()) {
		SetAnimStateMulticast(AnimState);
	}
}
	

bool Afps_cppCharacter::SetAnimStateServer_Validate(EAnimStateEnum AnimState)
{
	return true;
}

void Afps_cppCharacter::SetStatsToMulticast_Implementation(FWeaponStatsStruct CurrentStats)
{
	bCurrentStats = CurrentStats;
}

void Afps_cppCharacter::SetStatsToServer_Implementation(FWeaponStatsStruct CurrentStats)
{
	if (HasAuthority()) {
		SetStatsToMulticast(CurrentStats);
	}
	
}

bool Afps_cppCharacter::SetStatsToServer_Validate(FWeaponStatsStruct CurrentStats)
{
	return true;
}

void Afps_cppCharacter::PlayAnimMontageMulticast_Implementation(UAnimMontage* AnimMontage)
{
	PlayAnimMontage(AnimMontage);
}

void Afps_cppCharacter::PlayAnimMontageServer_Implementation(UAnimMontage* AnimMontage) 
{
	if (HasAuthority()) {
		PlayAnimMontageMulticast(AnimMontage);
	}
}

bool Afps_cppCharacter::PlayAnimMontageServer_Validate(UAnimMontage* AnimMontage)
{
	return true;
}

void Afps_cppCharacter::SpawnPickupActorMulticast_Implementation(FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, FDynamicInventoryItem Item, TSubclassOf<class APickUpBase> Class)
{
	if (Class)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = CollisionHandlingOverride;
		SpawnParams.Instigator = nullptr;

		APickUpBase* PickupActor = GetWorld()->SpawnActor<APickUpBase>(Class, SpawnTransform, SpawnParams);
		if (PickupActor)
		{
			PickupActor->SetItem(Item);
		}
	}
}

void Afps_cppCharacter::SpawnPickupActorServer_Implementation(FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, FDynamicInventoryItem Item, TSubclassOf<class APickUpBase> Class)
{
	if (HasAuthority())
	{
		SpawnPickupActorMulticast(SpawnTransform, CollisionHandlingOverride, Item, Class);
	}	
}

bool Afps_cppCharacter::SpawnPickupActorServer_Validate(FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, FDynamicInventoryItem Item, TSubclassOf<class APickUpBase> Class)
{
	return true;
}

void Afps_cppCharacter::SpawnBulletHoleMulticast_Implementation(FTransform SpawnTransform)
{
	if (GetWorld())
	{
		ABulletHole* NewActor = GetWorld()->SpawnActor<ABulletHole>(ABulletHole::StaticClass(), SpawnTransform);
	}
}

void Afps_cppCharacter::SpawnBulletHoleServer_Implementation(FTransform SpawnTransform)
{
	if (HasAuthority())
	{
		SpawnBulletHoleMulticast(SpawnTransform);
	}
}

bool Afps_cppCharacter::SpawnBulletHoleServer_Validate(FTransform SpawnTransform)
{
	return true;
}

void Afps_cppCharacter::ApplyDamageServer_Implementation(AActor* DamageActor, float BaseDamage, AActor* DamageCauser)
{
	UGameplayStatics::ApplyDamage(DamageActor, BaseDamage, nullptr, DamageCauser, nullptr);
}

void Afps_cppCharacter::WallDistanceMulticast_Implementation(float WallDistance)
{
	bWallDistance = WallDistance;
}
void Afps_cppCharacter::WallDistanceServer_Implementation(float WallDistance)
{
	if (HasAuthority()) 
	{
		WallDistanceMulticast(WallDistance);
	}
	
}

float Afps_cppCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.0f)
	{
		bHealth -= ActualDamage;

		bHealth = FMath::Max(bHealth, 0.0f);
		if (bHealth <= 0.0f)
		{
			Destroy();
		}
	}

	return ActualDamage;
}

void Afps_cppCharacter::SetLeanLeftServer_Implementation(bool LeanLeft)
{
	if (HasAuthority())
	{
		bLeanLeft = LeanLeft;
		Multicast_SetLeanLeftBooleans(bLeanLeft);
	}
}

bool Afps_cppCharacter::SetLeanLeftServer_Validate(bool LeanLeft)
{
	return true;
}


void Afps_cppCharacter::SetLeanRightServer_Implementation(bool LeanRight)
{
	if (HasAuthority())
	{
		bLeanRight = LeanRight;
		Multicast_SetLeanRightBooleans(bLeanRight);
	}
}

bool Afps_cppCharacter::SetLeanRightServer_Validate(bool LeanRight)
{
	return true;
}
void Afps_cppCharacter::Multicast_SetLeanLeftBooleans_Implementation(bool Left)
{
	bLeanLeft = Left;
}

void Afps_cppCharacter::Multicast_SetLeanRightBooleans_Implementation(bool Right)
{
	bLeanRight = Right;
}


void Afps_cppCharacter::PlayShotSequenceMulticast_Implementation(EItemTypeEnum WeaponType)
{
	if (WeaponType == EItemTypeEnum::Rifle)
	{
		AWeapon_Base_M4* M4 = Cast<AWeapon_Base_M4>(WeaponBase->GetChildActor());
		M4->GetSkeletalMeshComponent()->PlayAnimation(M4->GetShotSequence(), false);
	}
	else if (WeaponType == EItemTypeEnum::Pistol)
	{
		AWeapon_Base_Pistol* Pistol = Cast<AWeapon_Base_Pistol>(WeaponBase->GetChildActor());
		Pistol->GetSkeletalMeshComponent()->PlayAnimation(Pistol->GetShotSequence(), false);
	}
}

void Afps_cppCharacter::PlayShotSequenceServer_Implementation(EItemTypeEnum WeaponType)
{
	if (HasAuthority())
	{
		PlayShotSequenceMulticast(WeaponType);
	}
}

bool Afps_cppCharacter::PlayShotSequenceServer_Validate(EItemTypeEnum WeaponType)
{
	return true;
}

void Afps_cppCharacter::PlayReloadSequenceMulticast_Implementation(EItemTypeEnum WeaponType)
{
	if (WeaponType == EItemTypeEnum::Rifle)
	{
		AWeapon_Base_M4* M4 = Cast<AWeapon_Base_M4>(WeaponBase->GetChildActor());
		M4->GetSkeletalMeshComponent()->PlayAnimation(M4->GetReloadSequence(), false);
	}
	else if (WeaponType == EItemTypeEnum::Pistol)
	{
		AWeapon_Base_Pistol* Pistol = Cast<AWeapon_Base_Pistol>(WeaponBase->GetChildActor());
		Pistol->GetSkeletalMeshComponent()->PlayAnimation(Pistol->GetReloadSequence(), false);
	}
}

void Afps_cppCharacter::PlayReloadSequenceServer_Implementation(EItemTypeEnum WeaponType)
{
	if (HasAuthority())
	{
		PlayReloadSequenceMulticast(WeaponType);
	}
}

bool Afps_cppCharacter::PlayReloadSequenceServer_Validate(EItemTypeEnum WeaponType)
{
	return true;
}

void Afps_cppCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// CurrentWeaponClass를 네트워크에서 복제되도록 설정
	DOREPLIFETIME(Afps_cppCharacter, bCurrentWeaponClass);

	DOREPLIFETIME(Afps_cppCharacter, bAnimState);

	DOREPLIFETIME(Afps_cppCharacter, bLeanLeft);
	DOREPLIFETIME(Afps_cppCharacter, bLeanRight);

	DOREPLIFETIME(Afps_cppCharacter, bHealth);
}

void Afps_cppCharacter::IF_GetLeftHandSocketTransform_Implementation(FTransform& OutTransform)
{
	AActor* ChildActor = WeaponBase->GetChildActor();
	if (!ChildActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChildActor is null"));
		return;
	}

	if (!IsValid(ChildActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("ChildActor is pending kill"));
		return;
	}

	AWeapon_Base* Weapon_Base = Cast<AWeapon_Base>(ChildActor);
	if (!IsValid(Weapon_Base))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to cast ChildActor to AWeapon_Base"));
		return;
	}

	USkeletalMeshComponent* WeaponMesh = Weapon_Base->GetSkeletalMeshComponent();
	if (!IsValid(WeaponMesh))
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponMesh is null"));
		return;
	}

	if (!WeaponMesh->DoesSocketExist(FName("LHIK")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket LHIK does not exist"));
		return;
	}

	if (!GetMesh()->DoesSocketExist(FName("GrapSocket")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket GrapSocket does not exist"));
		return;
	}

	FTransform WeaponSocketTransform = WeaponMesh->GetSocketTransform(FName("LHIK"), RTS_World);
	FVector OutPosition;
	FRotator OutRotation;
	GetMesh()->TransformToBoneSpace(FName("hand_r"), WeaponSocketTransform.GetLocation(), FRotator(WeaponSocketTransform.GetRotation()), OutPosition, OutRotation);

	OutTransform.SetLocation(OutPosition);
	OutTransform.SetRotation(FQuat(OutRotation));
	OutTransform.SetScale3D(FVector(1, 1, 1));
}

void Afps_cppCharacter::IF_GetHandSwayFloats_Implementation(float& SideMove, float& MouseX, float& MouseY)
{
	SideMove = bSideMove;
	MouseX = bMouseX;
	MouseY = bMouseY;
}

void Afps_cppCharacter::IF_GetIsAim_Implementation(bool& Aim)
{
	Aim = bIsAiming;
}

void Afps_cppCharacter::IF_GetStopLeftHandIK_Implementation(bool& StopIK)
{
	StopIK = bStopLeftHandIK;

}

void Afps_cppCharacter::IF_GetWallDistance_Implementation(float& Value)
{
	Value = bWallDistance;
}
bool Afps_cppCharacter::Server_DeleteItem_Validate(AActor* ItemToDelete)
{
	return true;
}

void Afps_cppCharacter::Server_DeleteItem_Implementation(AActor* ItemToDelete)
{
	if (ItemToDelete)
	{
		ItemToDelete->Destroy();
	}
}

void Afps_cppCharacter::IF_AddItemToInventory_Implementation(const FDynamicInventoryItem Item, AActor* pickUp)
{
	if (IsLocallyControlled() && InventoryComponent->GetInventory().Num() <= InventoryComponent->GetMaxItemCount()) {
		InventoryComponent->GetInventory().Add(Item);
		if (pickUp) {
			Server_DeleteItem(pickUp);
			bCurrentItemSelection = 0;
			EquipItem();
		}
	}
}

void Afps_cppCharacter::IF_GetAnimState_Implementation(EAnimStateEnum& AnimState)
{
	AnimState = bAnimState;
}

void Afps_cppCharacter::IF_GetAimAlpha_Implementation(float& A)
{
	A = bAimAlpha;
}

void Afps_cppCharacter::IF_GetLeanBooleans_Implementation(bool& Left, bool& Right)
{
	Left = bLeanLeft;
	Right = bLeanRight;
}

void Afps_cppCharacter::IF_ReceiveProjectileImpact_Implementation(AActor* HitActor, UActorComponent* HitComponent, const FVector HitLocation, const FVector NormalPoint)
{
	ReceiveImpactProjectile(HitActor, HitComponent, HitLocation, NormalPoint);
}

