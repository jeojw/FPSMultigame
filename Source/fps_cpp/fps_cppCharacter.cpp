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
#include "Particles/ParticleSystem.h"
#include "BloodDecal.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon_Base_Pistol.h"
#include "Weapon_Base_M4.h"
#include "fps_cppPlayerController.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

Afps_cppCharacter::Afps_cppCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	OverrideInputComponentClass = UEnhancedInputComponent::StaticClass();

	if (GetMesh())
	{
		GetMesh()->DestroyComponent();
	}

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetRelativeLocationAndRotation(FVector(0, 0, -95.0f), FQuat(FRotator(0, -90.0f, 0)));
	BodyMesh->SetOwnerNoSee(true);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> PlayerMeshFinder(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny"));
	if (PlayerMeshFinder.Succeeded())
	{
		BodyMesh->SetSkeletalMeshAsset(PlayerMeshFinder.Object);
	}

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(BodyMesh, FName("neck_02"));
	CameraBoom->TargetArmLength = 0.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->SetRelativeLocation(FVector(0, 0, 0));
	FollowCamera->SetRelativeRotation(FQuat(FRotator(0, 0, 0)));

	WeaponBase = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponBase"));
	WeaponBase->SetupAttachment(BodyMesh, FName(TEXT("WeaponSocket")));
	WeaponBase->SetRelativeLocationAndRotation(FVector(-5.71417f, 2.633132f, -2.075797f), FQuat(FRotator(14.103956f, 91.855026f, -8.38177f)));
	WeaponBase->SetIsReplicated(true);
	WeaponBase->SetChildActorClass(AWeapon_Base_M4::StaticClass());

	OriginMeshVector = FVector(-11.0f, -4.686809f, -147.949827f);

	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPSMesh"));
	FPSMesh->SetupAttachment(FollowCamera);
	FPSMesh->SetOnlyOwnerSee(true);
	FPSMesh->SetCastShadow(false);
	FPSMesh->SetRelativeLocationAndRotation(OriginMeshVector, FQuat(FRotator(0, -90.0f, 0)));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FPSMeshFinder(TEXT("/Game/Characters/Mannequins/Meshes/SK_Mannequin_Arms"));
	if (FPSMeshFinder.Succeeded())
	{
		FPSMesh->SetSkeletalMeshAsset(FPSMeshFinder.Object);
	}

	FPSWeaponBase = CreateDefaultSubobject<UChildActorComponent>(TEXT("FPSWeaponBase"));
	FPSWeaponBase->SetupAttachment(FPSMesh, FName(TEXT("WeaponSocket")));
	FPSWeaponBase->SetRelativeLocationAndRotation(FVector(-6.916288f, 5.297911f, -1.021789f), FQuat(FRotator(13.512249f, 94.096334f, -7.351546f)));
	FPSWeaponBase->SetIsReplicated(true);
	FPSWeaponBase->SetChildActorClass(AWeapon_Base_M4::StaticClass());

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;

	bWalking = false;

	bSprinting = false;
	bSoundPlaying = false;
	bIsAiming = false;
	bStopLeftHandIK = false;
	bIsAttacking = false;
	bIsReloading = false;

	CurrentWeaponClass = AWeapon_Base_M4::StaticClass();
	CurrentWeaponType = EItemTypeEnum::Rifle;
	CurrentAnimState = EAnimStateEnum::Rifle;

	GetCharacterMovement()->MaxWalkSpeed = 350.0f;

	Tags.Add(FName("Flesh"));
	Tags.Add(FName("Player"));

	M4Location = FVector(-6.916288f, 5.297911f, -1.021789f);
	M4Rotation = FRotator(18.151713f, 82.151696f, -11.221878f);

	PistolLocation = FVector(-6.916288f, 5.297911f, -1.021789f);
	PistolRotation = FRotator(13.512249f, 82.151696f, -7.351546f);

	DT_ItemData = LoadObject<UDataTable>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/inventory/ItemData/DT_ItemData"));

	static ConstructorHelpers::FObjectFinder< UInputMappingContext> IMC_DefualtFinder(TEXT("/Game/ThirdPerson/Input/IMC_Default"));
	if (IMC_DefualtFinder.Succeeded())
	{
		DefaultMappingContext = IMC_DefualtFinder.Object;
	}

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
		BodyMesh->SetAnimInstanceClass(BlueprintAnim.Class);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> FPSBlueprintAnim(TEXT("/Game/Characters/Mannequins/Animations/ABP_ArmManny"));
	if (FPSBlueprintAnim.Succeeded())
	{
		FPSMesh->SetAnimInstanceClass(FPSBlueprintAnim.Class);
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

//------------------------------------------------------------------------ input actions
	
	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionAsset(TEXT("/Game/ThirdPerson/Input/Actions/IA_Jump"));
	if (JumpActionAsset.Succeeded())
	{
		JumpAction = JumpActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionAsset(TEXT("/Game/ThirdPerson/Input/Actions/IA_Move"));
	if (MoveActionAsset.Succeeded())
	{
		MoveAction = MoveActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> DropActionAsset(TEXT("/Game/ThirdPerson/Input/Actions/IA_DropItem"));
	if (DropActionAsset.Succeeded())
	{
		DropItemAction = DropActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>SprintActionAsset(TEXT("/Game/ThirdPerson/Input/Actions/IA_Sprint"));
	if (SprintActionAsset.Succeeded())
	{
		SprintAction = SprintActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>SwitchActionAsset(TEXT("/Game/ThirdPerson/Input/Actions/IA_Switch"));
	if (SwitchActionAsset.Succeeded())
	{
		SwitchAction = SwitchActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>LeanActionAsset(TEXT("/Game/ThirdPerson/Input/Actions/IA_Lean"));
	if (LeanActionAsset.Succeeded())
	{
		LeanAction = LeanActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>AimActionAsset(TEXT("/Game/ThirdPerson/Input/Actions/IA_Aim"));
	if (AimActionAsset.Succeeded())
	{
		AimAction = AimActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>LookActionAsset(TEXT("/Game/ThirdPerson/Input/Actions/IA_Look"));
	if (LookActionAsset.Succeeded())
	{
		LookAction = LookActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>FireActionAsset(TEXT("/Game/ThirdPerson/Input/Actions/IA_Fire"));
	if (FireActionAsset.Succeeded())
	{
		FireAction = FireActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>ReloadActionAsset(TEXT("/Game/ThirdPerson/Input/Actions/IA_Reload"));
	if (ReloadActionAsset.Succeeded())
	{
		ReloadAction = ReloadActionAsset.Object;
	}

//------------------------------------------------------------------------

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
	
	EquipItem(0);

	InitializePlayerState();

	Afps_cppPlayerController* PlayerController = Cast<Afps_cppPlayerController>(NewController);
	if (PlayerController != nullptr)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem != nullptr)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// ������ ����
	if (Afps_cppPlayerController* PC = Cast<Afps_cppPlayerController>(NewController))
	{
		SetOwner(PC);

		PlayerInterface.SetInterface(Cast<IPlayerInterface>(this));
		PlayerInterface.SetObject(this);
	}
}

void Afps_cppCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	InitializePlayerState();

	EquipItem(0);

	if (FPSWeaponBase && WeaponBase)
	{
		if (AActor* FPSChild = FPSWeaponBase->GetChildActor())
		{
			FPSChild->SetOwner(this);
		}

		if (AActor* Child = WeaponBase->GetChildActor())
		{
			Child->SetOwner(this);
		}
	}

	Afps_cppPlayerController* PlayerController = Cast<Afps_cppPlayerController>(GetController());
	if (PlayerController)
	{
		UE_LOG(LogTemp, Log, TEXT("BeginPlay: Character controlled by PlayerController"));
		SetOwner(PlayerController);

		// Initialize the PlayerInterface
		PlayerInterface.SetInterface(Cast<IPlayerInterface>(this));
		PlayerInterface.SetObject(this);

		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem != nullptr)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
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
	}

	if (FollowCamera)
	{
		GetWorldTimerManager().SetTimer(CheckWallTimerHandle, this, &Afps_cppCharacter::CheckWallTick, 0.1f, true);
	}
}

void Afps_cppCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FPSPlayerState)
	{
		InitializePlayerState();
	}

	GetWorldTimerManager().SetTimer(bTimerHandle_CheckWallTick, this, &Afps_cppCharacter::CheckWallTick, 0.02f, true);

	bRecoilTimeline.TickTimeline(DeltaTime);
	bAimTimeline.TickTimeline(DeltaTime);

	if (FPSPlayerState && FPSPlayerState->GetIsDead())
	{
		if (HasAuthority())
		{
			DeactivateObjectMulticast();
		}
		else
		{
			DeactivateObjectServer();
		}
	}

	if (!FMath::IsNearlyZero(GetVelocity().Size()) && FPSPlayerState && !FPSPlayerState->GetIsDead())
	{
		if (!bJumping)
		{
			if (GetCharacterMovement()->MaxWalkSpeed < 600.0f)
			{
				GetWorld()->GetTimerManager().ClearTimer(RunTimerHandle);
				if (HasAuthority())
				{
					PlaySoundWithCooldownMulticast(GetActorLocation(), MetalWalkSoundCue, 1.0f);
				}
				else
				{
					PlaySoundWithCooldownServer(GetActorLocation(), MetalWalkSoundCue, 1.0f);
				}
			}
			else
			{
				GetWorld()->GetTimerManager().ClearTimer(WalkTimerHandle);
				if (HasAuthority())
				{
					PlaySoundWithCooldownMulticast(GetActorLocation(), MetalRunSoundCue, 0.3f);
				}
				else
				{
					PlaySoundWithCooldownServer(GetActorLocation(), MetalRunSoundCue, 0.3f);
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

void Afps_cppCharacter::InitializePlayerState()
{
	FPSPlayerState = Cast<Afps_cppPlayerState>(GetPlayerState());
}

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
	Afps_cppPlayerController* PlayerController = Cast<Afps_cppPlayerController>(GetController());
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
	if (FPSPlayerState && !FPSPlayerState->GetIsDead())
	{
		Super::Jump();
		bJumping = true;

		if (HasAuthority())
		{
			PlaySoundAtLocationMulticast(GetActorLocation(), MetalJumpSoundCue);
		}
		else
		{
			PlaySoundAtLocationServer(GetActorLocation(), MetalJumpSoundCue);
		}
	}
	
}
void Afps_cppCharacter::StopJumping()
{
	if (FPSPlayerState && !FPSPlayerState->GetIsDead())
		Super::StopJumping();
}

void Afps_cppCharacter::Landed(const FHitResult& Hit)
{
	if (FPSPlayerState && !FPSPlayerState->GetIsDead())
	{
		Super::Landed(Hit);
		bJumping = false;
		if (HasAuthority())
		{
			PlaySoundAtLocationMulticast(GetActorLocation(), MetalLandSoundCue);
		}
		else
		{
			PlaySoundAtLocationServer(GetActorLocation(), MetalLandSoundCue);
		}
	}
}

void Afps_cppCharacter::Move(const FInputActionValue& Value)
{
	if (FPSPlayerState && !FPSPlayerState->GetIsDead())
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
}

void Afps_cppCharacter::Look(const FInputActionValue& Value)
{
	if (FPSPlayerState && !FPSPlayerState->GetIsDead())
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
}

void Afps_cppCharacter::Sprint()
{
	if (FPSPlayerState && !FPSPlayerState->GetIsDead())
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
	else
		bSprinting = false;
}

void Afps_cppCharacter::StopSprint()
{
	if (FPSPlayerState && !FPSPlayerState->GetIsDead())
	{
		bSprinting = false;

		if (HasAuthority())
		{
			if (GetCharacterMovement())
			{
				GetCharacterMovement()->MaxWalkSpeed = 350.0f;
			}
		}
		else
		{
			SprintServer(350.0f);
		}
	}
}

void Afps_cppCharacter::ApplyRecoil(float PitchValue, float YawValue)
{
	Afps_cppPlayerController* PlayerController = Cast<Afps_cppPlayerController>(GetController());
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
	float PitchValue = FMath::Lerp(0.0f, CurrentStats.InputRecoil, CurveValue);
	float YawValue = FMath::Lerp(0.0f, CurrentStats.InputRecoil, CurveValue);
	ApplyRecoil(PitchValue, YawValue);
}

void Afps_cppCharacter::ControlAim(float Value)
{
	if (FPSPlayerState && !FPSPlayerState->GetIsDead() && CurrentAnimState != EAnimStateEnum::Melee)
	{
		FVector AimMeshVector;
		FVector AimWeaponVector;
		FRotator AimWeaponRotator;
		FVector OriginWeaponVector;
		FRotator OriginWeaponRotator;
		float AimFieldOfView;
		if (CurrentWeaponType == EItemTypeEnum::Rifle)
		{
			AimMeshVector = FVector(OriginMeshVector.X, -20.05f, OriginMeshVector.Z);
			AimFieldOfView = 14.240001f;
			AimWeaponVector = FPSWeaponBase->GetRelativeLocation();
			AimWeaponRotator = FPSWeaponBase->GetRelativeRotation();
			OriginWeaponVector = M4Location;
			OriginWeaponRotator = M4Rotation;
		}
		else
		{
			AimMeshVector = OriginMeshVector;
			AimFieldOfView = 21.359997f;
			AimWeaponVector = FVector(-6.0f, 8.75f, 2.7f);
			AimWeaponRotator = FRotator(11.0f, 94.0f, -8.0f);
			OriginWeaponVector = PistolLocation;
			OriginWeaponRotator = PistolRotation;
		}
		if (FPSMesh && FPSMesh->IsValidLowLevel() &&
			FollowCamera && FollowCamera->IsValidLowLevel())
		{
			FVector NewMeshLocation = FMath::Lerp(OriginMeshVector, AimMeshVector, Value);
			FVector NewWeaponLocation = FMath::Lerp(OriginWeaponVector, AimWeaponVector, Value);
			FRotator NewWeaponRotation = FMath::Lerp(OriginWeaponRotator, AimWeaponRotator, Value);
			float NewFieldOfView = FMath::Lerp(90.0f, AimFieldOfView, Value);
			FPSMesh->SetRelativeLocation(NewMeshLocation);
			FPSWeaponBase->SetRelativeLocation(NewWeaponLocation);
			FPSWeaponBase->SetRelativeRotation(NewWeaponRotation);
			FollowCamera->SetFieldOfView(NewFieldOfView);
		}
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
	if (FPSPlayerState && !FPSPlayerState->GetIsDead())
	{
		bIsAttacking = true;

		switch (CurrentWeaponType)
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
	else
		bIsAttacking = false;
}

void Afps_cppCharacter::RifleFire()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(bFireCooldownTimer))
	{
		if (FPSPlayerState && bIsAttacking)
		{
			if (FPSPlayerState->GetCurrentBullet(0) >= 1)
			{
				GetWorld()->GetTimerManager().SetTimer(
					bFireCooldownTimer,
					this,
					&Afps_cppCharacter::ResetFireRifle,
					CurrentStats.FireRate,
					false
				);
				FPSPlayerState->ReduceCurrentBullet(0);
				if (FPSWeaponBase && FPSWeaponBase->GetChildActor())
				{
					UE_LOG(LogTemp, Warning, TEXT("Child Actor Class: %s"), *FPSWeaponBase->GetChildActor()->GetClass()->GetName());
					AWeapon_Base_M4* FPSWeapon = Cast<AWeapon_Base_M4>(FPSWeaponBase->GetChildActor());
					if (FPSWeapon)
					{
						if (!FPSWeapon->GetIsReplicated())
						{
							FPSWeapon->SetReplicates(true);
						}
						FTransform ShellTransform;
						IGunInterface::Execute_GetShellTransform(FPSWeapon, ShellTransform);
						EjectShell(ShellTransform.GetLocation(), FRotator(ShellTransform.GetRotation()));
						FireProjectileToDirection();

						if (BodyMesh->GetAnimInstance() &&
							FPSMesh->GetAnimInstance())
						{
							UFunction* F_Procedural_Recoil = BodyMesh->GetAnimInstance()->FindFunction(TEXT("f_ProceduralRecoil"));
							UFunction* F_Procedural_Recoil_FPS = FPSMesh->GetAnimInstance()->FindFunction(TEXT("f_ProceduralRecoil"));
							if (F_Procedural_Recoil && F_Procedural_Recoil_FPS)
							{
								BodyMesh->GetAnimInstance()->ProcessEvent(F_Procedural_Recoil, &CurrentStats.ProceduralRecoil);
								FPSMesh->GetAnimInstance()->ProcessEvent(F_Procedural_Recoil_FPS, &CurrentStats.ProceduralRecoil);
								if (HasAuthority())
								{
									PlayShotSequenceClient(EItemTypeEnum::Rifle);
									PlayShotSequenceMulticast(EItemTypeEnum::Rifle);
								}
								else
								{
									PlayShotSequenceServer(EItemTypeEnum::Rifle);
								}
								GetWorld()->GetTimerManager().SetTimer(bFireRateTimer, this, &Afps_cppCharacter::FireDelayCompleted, CurrentStats.FireRate, false);
							}
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("AnimInstace is nullptr"));
						}

					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("FPSWeapon is nullptr"));
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
		if (FPSPlayerState && bIsAttacking)
		{
			if (FPSPlayerState->GetCurrentBullet(1) >= 1)
			{
				GetWorld()->GetTimerManager().SetTimer(
					bFireCooldownTimer,
					this,
					&Afps_cppCharacter::ResetFireRifle,
					CurrentStats.FireRate,
					false
				);
				FPSPlayerState->ReduceCurrentBullet(1);
				if (FPSWeaponBase && FPSWeaponBase->GetChildActor())
				{
					AWeapon_Base_Pistol* FPSWeapon = Cast<AWeapon_Base_Pistol>(FPSWeaponBase->GetChildActor());
					if (FPSWeapon)
					{
						if (!FPSWeapon->GetIsReplicated())
						{
							FPSWeapon->SetReplicates(true);
						}

						FTransform ShellTransform;
						IGunInterface::Execute_GetShellTransform(FPSWeapon, ShellTransform);
						EjectShell(ShellTransform.GetLocation(), FRotator(ShellTransform.GetRotation()));
						FireProjectileToDirection();

						if (BodyMesh->GetAnimInstance() &&
							FPSMesh->GetAnimInstance())
						{
							UFunction* F_Procedural_Recoil = BodyMesh->GetAnimInstance()->FindFunction(TEXT("f_ProceduralRecoil"));
							UFunction* F_Procedural_Recoil_FPS = FPSMesh->GetAnimInstance()->FindFunction(TEXT("f_ProceduralRecoil"));
							if (F_Procedural_Recoil && F_Procedural_Recoil_FPS)
							{
								BodyMesh->GetAnimInstance()->ProcessEvent(F_Procedural_Recoil, &CurrentStats.ProceduralRecoil);
								FPSMesh->GetAnimInstance()->ProcessEvent(F_Procedural_Recoil_FPS, &CurrentStats.ProceduralRecoil);
								if (HasAuthority())
								{
									PlayShotSequenceClient(EItemTypeEnum::Pistol);
									PlayShotSequenceMulticast(EItemTypeEnum::Pistol);
								}
								else
								{
									PlayShotSequenceServer(EItemTypeEnum::Pistol);
								}
								GetWorld()->GetTimerManager().SetTimer(bFireRateTimer, this, &Afps_cppCharacter::FireDelayCompleted, CurrentStats.FireRate, false);
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
	if (FPSPlayerState && !FPSPlayerState->GetIsDead())
	{
		bIsAiming = true;
		bAimTimeline.Play();
	}
	else
		bIsAiming = false;
}

void Afps_cppCharacter::StopAiming()
{
	bIsAiming = false;
	bAimTimeline.Reverse();
}

void Afps_cppCharacter::FireDelayCompleted()
{
	if (FPSPlayerState && bIsAttacking && FPSPlayerState->GetCurrentBullet(bCurrentItemSelection) >= 1)
	{
		RifleFire();
	}
	
}

void Afps_cppCharacter::ReloadDelayCompleted()
{
	bIsAiming = true;
	bIsReloading = false;

	if (FPSPlayerState && FPSPlayerState->GetInventory() && FPSPlayerState->GetInventory()->GetInventory().IsValidIndex(bCurrentItemSelection))
	{
		FPSPlayerState->ReloadCurrentBullet(bCurrentItemSelection, CurrentStats.MagSize);
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

void Afps_cppCharacter::Reload()
{
	if (!bIsReloading && !FPSPlayerState->GetIsDead() && CurrentAnimState != EAnimStateEnum::Melee)
	{
		bIsReloading = true;
		if (!FPSPlayerState->GetInventory())
		{
			UE_LOG(LogTemp, Error, TEXT("Inventory is nullptr"));
			return;
		}
		if (FPSPlayerState->GetCurrentBullet(bCurrentItemSelection) < CurrentStats.MagSize)
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
			if (WeaponBase && WeaponBase->GetChildActor() && 
				FPSWeaponBase && FPSWeaponBase->GetChildActor())
			{
				if (CurrentReloadAnimation)
				{
					if (HasAuthority())
					{
						PlayAnimMontageMulticast(CurrentReloadAnimation);
					}
					else
					{
						PlayAnimMontageServer(CurrentReloadAnimation);
					}

					if (CurrentAnimState == EAnimStateEnum::Rifle)
					{
						if (HasAuthority())
						{
							PlayReloadSequenceClient(EItemTypeEnum::Rifle);
							PlayReloadSequenceMulticast(EItemTypeEnum::Rifle);
						}
						else
						{
							PlayReloadSequenceServer(EItemTypeEnum::Rifle);
						}
					}

					else if (CurrentAnimState == EAnimStateEnum::Pistol)
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
						CurrentStats.ReloadTime,
						false
					);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Montage is null"));
				}
			}
		}
	}
}

void Afps_cppCharacter::DropItem()
{
	if (FPSPlayerState && !FPSPlayerState->GetIsDead())
	{
		if (!FPSPlayerState->GetInventory()) {
			UE_LOG(LogTemp, Error, TEXT("Inventory is nullptr"));
			return;
		}
		if (FPSPlayerState->GetCurrentID(bCurrentItemSelection) > 0)
		{
			if (FPSPlayerState->GetInventory()->GetInventory().IsValidIndex(bCurrentItemSelection))
			{
				FDynamicInventoryItem Item = FPSPlayerState->GetItem(bCurrentItemSelection);
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
				FPSPlayerState->RemoveItemAt(bCurrentItemSelection);
				EquipItem(bCurrentItemSelection);
			}
		}
	}
}

void Afps_cppCharacter::DestroyWeapon()
{
	if (HasAuthority())
	{
		StopLeftHandIKMulticast(true);
	}
	else
	{
		StopLeftHandIKServer(true);
	}
	WeaponBase->DestroyChildActor();
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
	if (FPSPlayerState && !FPSPlayerState->GetIsDead())
	{
		Afps_cppPlayerController* PlayerController = Cast<Afps_cppPlayerController>(GetController());
		if (PlayerController)
		{
			if (PlayerController->IsInputKeyDown(EKeys::One))
			{
				bCurrentItemSelection = 0;
				EquipItem(bCurrentItemSelection);
			}
			else if (PlayerController->IsInputKeyDown(EKeys::Two))
			{
				bCurrentItemSelection = 1;
				EquipItem(bCurrentItemSelection);
			}
			else if (PlayerController->IsInputKeyDown(EKeys::Three))
			{
				bCurrentItemSelection = 2;
				EquipItem(bCurrentItemSelection);
			}
		}
	}
}

void Afps_cppCharacter::Lean(const FInputActionValue& Value)
{
	if (FPSPlayerState && !FPSPlayerState->GetIsDead())
	{
		Afps_cppPlayerController* PlayerController = Cast<Afps_cppPlayerController>(GetController());
		if (PlayerController)
		{
			float LeanValue = Value.Get<float>();
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
				CurrentLean = LeanValue * -15.0f;
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
				CurrentLean = 0;
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
				CurrentLean = LeanValue * 15.0f;
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
				CurrentLean = 0;
			}
		}
	}
	else
	{
		bLeanLeft = false;
		bLeanRight = false;
		CurrentLean = 0;
	}
}

void Afps_cppCharacter::EquipItem(int index)
{
	if (FPSPlayerState)
	{
		Afps_cppPlayerController* GetController = Cast<Afps_cppPlayerController>(GetOwner());
		if (GetController && GetController->IsLocalController()) {
			if (!FPSPlayerState->GetInventory()) {
				UE_LOG(LogTemp, Error, TEXT("Inventory is nullptr"));
				return;
			}

			int CurrentSelection = index;
			if (!FPSPlayerState->GetInventory()->GetInventory().IsValidIndex(CurrentSelection)) {
				UE_LOG(LogTemp, Error, TEXT("Invalid inventory index: %d"), CurrentSelection);
				return;
			}

			int id = FPSPlayerState->GetCurrentID(index);
			FString fname = FString::FromInt(id);
			if (DT_ItemData) {
				TArray<FName> RowNames = DT_ItemData->GetRowNames();
				for (int i = 0; i < RowNames.Num(); i++) {
					if (RowNames[i] == fname)
					{
						FItemDataTable* data = DT_ItemData->FindRow<FItemDataTable>(RowNames[i], RowNames[i].ToString());
						if (data)
						{
							bool bStop = (data->AnimState == EAnimStateEnum::Melee);
							if (HasAuthority())
							{
								SetWeaponDataMulticast(data->WeaponClass, data->Stats, data->AnimState, bStop, data->Type);
							}
							else
							{
								SetWeaponDataServer(data->WeaponClass, data->Stats, data->AnimState, bStop, data->Type);
							}
							SetCurrentReloadAnimation(data->ReloadAnimation);
							SetWeaponIcon(data->icon);
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
}

void Afps_cppCharacter::FireProjectileToDirection()
{
	FVector MuzzlePointLocalLocation;
	FTransform MuzzlePoint = FPSWeaponBase->GetSocketTransform(FName("MuzzlePoint"));
	MuzzlePointLocalLocation = MuzzlePoint.GetLocation();

	FVector Start = MuzzlePointLocalLocation;
	FVector End = Start + FollowCamera->GetForwardVector() * CurrentStats.Range;
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
	if (HasAuthority())
	{
		ApplyDamage(actor, CurrentStats.Damage, this);
		HandleImpactEffectsServer(actor, Loc, Normal);
	}
	else
	{
		ServerReceiveImpactProjectile(actor, comp, Loc, Normal);
	}
}

void Afps_cppCharacter::ServerReceiveImpactProjectile_Implementation(AActor* actor, UActorComponent* comp, FVector Loc, FVector Normal)
{
	ApplyDamage(actor, CurrentStats.Damage, this);
	HandleImpactEffectsServer(actor, Loc, Normal);
}

bool Afps_cppCharacter::ServerReceiveImpactProjectile_Validate(AActor* actor, UActorComponent* comp, FVector Loc, FVector Normal)
{
	return true;
}

void Afps_cppCharacter::HandleImpactEffectsServer_Implementation(AActor* actor, FVector Loc, FVector Normal)
{
	HandleImpactEffectsMulticast(actor, Loc, Normal);
}

bool Afps_cppCharacter::HandleImpactEffectsServer_Validate(AActor* actor, FVector Loc, FVector Normal)
{
	return true;
}

void Afps_cppCharacter::HandleImpactEffectsMulticast_Implementation(AActor* actor, FVector Loc, FVector Normal)
{
	HandleImpactEffects(actor, Loc, Normal);
}

void Afps_cppCharacter::HandleImpactEffects(AActor* actor, FVector Loc, FVector Normal)
{
	FTransform tmpTransform = FTransform(FRotationMatrix::MakeFromX(Normal).Rotator(), Loc, FVector(1, 1, 1));

	SpawnBulletHoleMulticast(tmpTransform);

	USoundCue* WeaponSound = nullptr;
	if (CurrentWeaponClass == AWeapon_Base_M4::StaticClass())
	{
		WeaponSound = RifleSurfaceImpactSoundCue;
	}
	else if (CurrentWeaponClass == AWeapon_Base_Pistol::StaticClass())
	{
		WeaponSound = PistolSurfaceImpactSoundCue;
	}
	
	if (actor->ActorHasTag("Metal"))
	{
		PlaySoundAtLocationMulticast(FollowCamera->GetComponentLocation(), WeaponSound);
		SpawnEmitterAtLocationMulticast(MetalImpactParticleSystem, Loc, FRotator(0, 0, 0), FVector(1, 1, 1));
	}
	else if (actor->ActorHasTag("Stone"))
	{
		PlaySoundAtLocationMulticast(FollowCamera->GetComponentLocation(), WeaponSound);
		SpawnEmitterAtLocationMulticast(StoneImpactParticleSystem, Loc, FRotator(0, 0, 0), FVector(1, 1, 1));
	}
	else if (actor->ActorHasTag("Player"))
	{
		PlaySoundAtLocationMulticast(FollowCamera->GetComponentLocation(), WeaponSound);
		SpawnEmitterAtLocationMulticast(MetalImpactParticleSystem, Loc, FRotator(0, 0, 0), FVector(1, 1, 1));

		FVector Start = Loc;
		FVector End = FollowCamera->GetForwardVector() * 1500.0f + Loc;
		FCollisionQueryParams TraceParams(FName(TEXT("HitTrace")), true, this);
		TraceParams.AddIgnoredActor(actor);
		FHitResult HitResult;

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);

		if (bHit)
		{
			FTransform tmpTransform2 = FTransform(FRotationMatrix::MakeFromX(HitResult.Normal).Rotator(), HitResult.Location, FVector(1, 1, 1));
			TSubclassOf<AActor> Blood = ABloodDecal::StaticClass();
			if (Blood)
			{
				SpawnActorToMulticast(Blood, tmpTransform2, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			}
		}
	}
}

void Afps_cppCharacter::DeleteItemServer_Implementation(AActor* DeleteItem)
{
	if (GetLocalRole() < ROLE_Authority) {
		Afps_cppPlayerController* PlayerController = Cast<Afps_cppPlayerController>(GetController());

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

void Afps_cppCharacter::PlaySoundAtLocationMulticast_Implementation(FVector Location, USoundBase* Sound)
{
	if (GetWorld())
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, Location);
	}
}

void Afps_cppCharacter::PlaySoundWithCooldownServer_Implementation(FVector Location, USoundBase* Sound, float Delay)
{
	PlaySoundWithCooldownMulticast(Location, Sound, Delay);
}

void Afps_cppCharacter::PlaySoundWithCooldownMulticast_Implementation(FVector Location, USoundBase* Sound, float Delay)
{
	if (GetWorld())
	{
		FTimerHandle& TimerHandle = (GetCharacterMovement()->MaxWalkSpeed < 600.0f) ? WalkTimerHandle : RunTimerHandle;
		// TimerHandle �ʱ�ȭ
		if (!TimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, &TimerHandle]()
				{
					// Ÿ�̸Ӱ� ����Ǿ��� �� ���� ��� ���¸� false�� ����
					if (TimerHandle.IsValid())
					{
						bSoundPlaying = false;
					}
				}), Delay, true);

			// ���� ���
		}
		if (!bSoundPlaying)
		{
			bSoundPlaying = true;
			if (HasAuthority())
			{
				PlaySoundAtLocationMulticast(Location, Sound);
			}
			else
			{
				PlaySoundAtLocationServer(Location, Sound);
			}
		}
	}
}

void Afps_cppCharacter::PlaySoundAtLocationServer_Implementation(FVector Location, USoundBase* Sound)
{
	PlaySoundAtLocationMulticast(Location, Sound);	
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
	SpawnEmitterAtLocationMulticast(EmitterTemplate, Location, Rotation, Scale);
}


void Afps_cppCharacter::SpawnActorToMulticast_Implementation(TSubclassOf<AActor> Class, FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride)
{
	UWorld* World = GetWorld();
	if (World && Class)
	{
		// Actor�� Spawn�մϴ�.
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = CollisionHandlingOverride;
		SpawnParams.Instigator = GetInstigator();

		AActor* SpawnedActor = World->SpawnActor<AActor>(Class, SpawnTransform, SpawnParams);
	}
}
void Afps_cppCharacter::SpawnActorToServer_Implementation(TSubclassOf<AActor> Class, FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride)
{
	SpawnActorToMulticast(Class, SpawnTransform, CollisionHandlingOverride);
}

void Afps_cppCharacter::PlayAnimMontageMulticast_Implementation(UAnimMontage* AnimMontage)
{
	BodyMesh->GetAnimInstance()->Montage_Play(AnimMontage);
	FPSMesh->GetAnimInstance()->Montage_Play(AnimMontage);
}

void Afps_cppCharacter::PlayAnimMontageServer_Implementation(UAnimMontage* AnimMontage) 
{
	PlayAnimMontageMulticast(AnimMontage);
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
	SpawnPickupActorMulticast(SpawnTransform, CollisionHandlingOverride, Item, Class);
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

void Afps_cppCharacter::ApplyDamage_Implementation(AActor* DamageActor, float BaseDamage, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("ApplyDamage called"));
	AController* InstigatorController = DamageCauser->GetInstigatorController();

	UGameplayStatics::ApplyDamage(DamageActor, BaseDamage, InstigatorController, DamageCauser, nullptr);
}


void Afps_cppCharacter::WallDistanceMulticast_Implementation(float WallDistance)
{
	bWallDistance = WallDistance;
}
void Afps_cppCharacter::WallDistanceServer_Implementation(float WallDistance)
{
	WallDistanceMulticast(WallDistance);
}

float Afps_cppCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (ActualDamage > 0.0f)
	{
		if (FPSPlayerState)
		{
			FPSPlayerState->ReduceHealth(ActualDamage);

			FPSPlayerState->SetHealth(FMath::Max(FPSPlayerState->GetHealth(), 0.0f));

			if (FPSPlayerState->GetHealth() <= 0.0f)
			{
				FPSPlayerState->SetIsDead(true);
			}
		}
	}

	return ActualDamage;
}

void Afps_cppCharacter::SetLeanLeftServer_Implementation(bool LeanLeft)
{
	bLeanLeft = LeanLeft;
	Multicast_SetLeanLeftBooleans(bLeanLeft);
}

bool Afps_cppCharacter::SetLeanLeftServer_Validate(bool LeanLeft)
{
	return true;
}


void Afps_cppCharacter::SetLeanRightServer_Implementation(bool LeanRight)
{
	bLeanRight = LeanRight;
	Multicast_SetLeanRightBooleans(bLeanRight);
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

void Afps_cppCharacter::PlayShotSequenceClient_Implementation(EItemTypeEnum WeaponType)
{
	if (WeaponType == EItemTypeEnum::Rifle)
	{
		AWeapon_Base_M4* FM4 = Cast<AWeapon_Base_M4>(FPSWeaponBase->GetChildActor());
		if (FM4)
		{
			FM4->GetSkeletalMeshComponent()->PlayAnimation(FM4->GetShotSequence(), false);
		}

	}
	else if (WeaponType == EItemTypeEnum::Pistol)
	{
		AWeapon_Base_Pistol* FPistol = Cast<AWeapon_Base_Pistol>(FPSWeaponBase->GetChildActor());
		if (FPistol)
		{
			FPistol->GetSkeletalMeshComponent()->PlayAnimation(FPistol->GetShotSequence(), false);
		}
	}
}

void Afps_cppCharacter::PlayShotSequenceMulticast_Implementation(EItemTypeEnum WeaponType)
{
	if (WeaponType == EItemTypeEnum::Rifle)
	{
		AWeapon_Base_M4* M4 = Cast<AWeapon_Base_M4>(WeaponBase->GetChildActor());
		if (M4)
		{
			M4->GetSkeletalMeshComponent()->PlayAnimation(M4->GetShotSequence(), false);
			M4->GetSkeletalMeshComponent()->SetOwnerNoSee(true);
			M4->GetSkeletalMeshComponent()->SetOnlyOwnerSee(false);
		}	
	}
	else if (WeaponType == EItemTypeEnum::Pistol)
	{
		AWeapon_Base_Pistol* Pistol = Cast<AWeapon_Base_Pistol>(WeaponBase->GetChildActor());
		if (Pistol)
		{
			Pistol->GetSkeletalMeshComponent()->PlayAnimation(Pistol->GetShotSequence(), false);
			Pistol->GetSkeletalMeshComponent()->SetOwnerNoSee(true);
			Pistol->GetSkeletalMeshComponent()->SetOnlyOwnerSee(false);
		}
	}
}

void Afps_cppCharacter::PlayShotSequenceServer_Implementation(EItemTypeEnum WeaponType)
{
	PlayShotSequenceClient(WeaponType);
	PlayShotSequenceMulticast(WeaponType);
}

bool Afps_cppCharacter::PlayShotSequenceServer_Validate(EItemTypeEnum WeaponType)
{
	return true;
}

void Afps_cppCharacter::PlayReloadSequenceClient_Implementation(EItemTypeEnum WeaponType)
{
	if (WeaponType == EItemTypeEnum::Rifle)
	{
		AWeapon_Base_M4* FM4 = Cast<AWeapon_Base_M4>(FPSWeaponBase->GetChildActor());
		if (FM4)
		{
			FM4->GetSkeletalMeshComponent()->PlayAnimation(FM4->GetReloadSequence(), false);
		}
	}
	else if (WeaponType == EItemTypeEnum::Pistol)
	{
		AWeapon_Base_Pistol* FPistol = Cast<AWeapon_Base_Pistol>(FPSWeaponBase->GetChildActor());
		if (FPistol)
		{
			FPistol->GetSkeletalMeshComponent()->PlayAnimation(FPistol->GetReloadSequence(), false);
		}
	}
}

void Afps_cppCharacter::PlayReloadSequenceMulticast_Implementation(EItemTypeEnum WeaponType)
{
	if (WeaponType == EItemTypeEnum::Rifle)
	{
		AWeapon_Base_M4* M4 = Cast<AWeapon_Base_M4>(WeaponBase->GetChildActor());
		if (M4)
		{
			M4->GetSkeletalMeshComponent()->PlayAnimation(M4->GetReloadSequence(), false);
			M4->GetSkeletalMeshComponent()->SetOwnerNoSee(true);
			M4->GetSkeletalMeshComponent()->SetOnlyOwnerSee(false);
		}
	}
	else if (WeaponType == EItemTypeEnum::Pistol)
	{
		AWeapon_Base_Pistol* Pistol = Cast<AWeapon_Base_Pistol>(WeaponBase->GetChildActor());
		if (Pistol)
		{
			Pistol->GetSkeletalMeshComponent()->PlayAnimation(Pistol->GetReloadSequence(), false);
			Pistol->GetSkeletalMeshComponent()->SetOwnerNoSee(true);
			Pistol->GetSkeletalMeshComponent()->SetOnlyOwnerSee(false);
		}
	}
}

void Afps_cppCharacter::PlayReloadSequenceServer_Implementation(EItemTypeEnum WeaponType)
{
	PlayReloadSequenceClient(WeaponType);
	PlayReloadSequenceMulticast(WeaponType);
}

bool Afps_cppCharacter::PlayReloadSequenceServer_Validate(EItemTypeEnum WeaponType)
{
	return true;
}

void Afps_cppCharacter::ActivateObjectMulticast_Implementation()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->GravityScale = 1.0f;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	if (FPSPlayerState)
	{
		FPSPlayerState->SetHealth(FPSPlayerState->GetMaxHealth());
		FPSPlayerState->SetIsDead(false);
	}
}

void Afps_cppCharacter::ActivateObjectServer_Implementation()
{
	ActivateObjectMulticast();
}

bool Afps_cppCharacter::ActivateObjectServer_Validate()
{
	return true;
}

void Afps_cppCharacter::DeactivateObjectMulticast_Implementation()
{
	DestroyWeapon();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0.0f;
	GetCharacterMovement()->DisableMovement();
}

void Afps_cppCharacter::DeactivateObjectServer_Implementation()
{
	DeactivateObjectMulticast();
}

bool Afps_cppCharacter::DeactivateObjectServer_Validate()
{
	return true;
}

void Afps_cppCharacter::SetWeaponDataServer_Implementation(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType)
{
	SetWeaponDataMulticast(WeaponClass, Stats, AnimState, bStop, WeaponType);
}

bool Afps_cppCharacter::SetWeaponDataServer_Validate(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType)
{
	return true;
}

void Afps_cppCharacter::SetWeaponDataMulticast_Implementation(TSubclassOf<AActor> WeaponClass, FWeaponStatsStruct Stats, EAnimStateEnum AnimState, bool bStop, EItemTypeEnum WeaponType)
{
	CurrentWeaponClass = WeaponClass;
	CurrentStats = Stats;
	CurrentAnimState = AnimState;
	bStopLeftHandIK = bStop;
	CurrentWeaponType = WeaponType;

	if (FPSWeaponBase && WeaponBase && CurrentWeaponClass)
	{
		FPSWeaponBase->SetChildActorClass(CurrentWeaponClass);
        WeaponBase->SetChildActorClass(CurrentWeaponClass);

        AActor* WeaponBaseChild = WeaponBase->GetChildActor();
        if (WeaponBaseChild)
        {
            // WeaponBaseChild에서 USkeletalMeshComponent를 가져와서 설정
            USkeletalMeshComponent* SkeletalMeshComponent = WeaponBaseChild->FindComponentByClass<USkeletalMeshComponent>();
            if (SkeletalMeshComponent)
            {
				if (IsLocallyControlled())
				{
					SkeletalMeshComponent->SetOwnerNoSee(true);
					SkeletalMeshComponent->SetOnlyOwnerSee(false);
				}
				else
				{
					SkeletalMeshComponent->SetOwnerNoSee(false);
					SkeletalMeshComponent->SetOnlyOwnerSee(false);
				}
            }
        }

        AActor* FPSWeaponBaseChild = FPSWeaponBase->GetChildActor();
        if (FPSWeaponBaseChild)
        {
            if (CurrentWeaponType == EItemTypeEnum::Rifle)
            {
                FPSWeaponBase->SetRelativeLocation(M4Location);
                FPSWeaponBase->SetRelativeRotation(M4Rotation);
            }
            else if (CurrentWeaponType == EItemTypeEnum::Pistol)
            {
                FPSWeaponBase->SetRelativeLocation(PistolLocation);
                FPSWeaponBase->SetRelativeRotation(PistolRotation);
            }

            // FPSWeaponBaseChild에서 USkeletalMeshComponent를 가져와서 설정
            USkeletalMeshComponent* SkeletalMeshComponent = FPSWeaponBaseChild->FindComponentByClass<USkeletalMeshComponent>();
            if (SkeletalMeshComponent)
            {
				if (IsLocallyControlled())
				{
					SkeletalMeshComponent->SetOwnerNoSee(false);
					SkeletalMeshComponent->SetOnlyOwnerSee(true);
				}
				else
				{
					SkeletalMeshComponent->SetOwnerNoSee(false);
					SkeletalMeshComponent->SetOnlyOwnerSee(false);
				}
            }
        }
	}
}

void Afps_cppCharacter::StopLeftHandIKMulticast_Implementation(bool bStop)
{
	bStopLeftHandIK = bStop;
}

void Afps_cppCharacter::StopLeftHandIKServer_Implementation(bool bStop)
{
	StopLeftHandIKMulticast(bStop);
}

bool Afps_cppCharacter::StopLeftHandIKServer_Validate(bool bStop)
{
	return true;
}

void Afps_cppCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(Afps_cppCharacter, bSoundPlaying);
	DOREPLIFETIME(Afps_cppCharacter, CurrentWeaponType);
	DOREPLIFETIME(Afps_cppCharacter, CurrentAnimState);
	DOREPLIFETIME(Afps_cppCharacter, CurrentStats);
	DOREPLIFETIME(Afps_cppCharacter, CurrentWeaponClass);

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

	if (!BodyMesh->DoesSocketExist(FName("GrapSocket")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket GrapSocket does not exist"));
		return;
	}

	FTransform WeaponSocketTransform = WeaponMesh->GetSocketTransform(FName("LHIK"), RTS_World);
	FVector OutPosition;
	FRotator OutRotation;
	BodyMesh->TransformToBoneSpace(FName("hand_r"), WeaponSocketTransform.GetLocation(), FRotator(WeaponSocketTransform.GetRotation()), OutPosition, OutRotation);

	OutTransform.SetLocation(OutPosition);
	OutTransform.SetRotation(FQuat(OutRotation));
	OutTransform.SetScale3D(FVector(1, 1, 1));
}

void Afps_cppCharacter::IF_GetLeftHandSocketTransform_FPS_Implementation(FTransform& OutTransform)
{
	AActor* FPSChildActor = FPSWeaponBase->GetChildActor();
	if (!FPSChildActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("FPS_ChildActor is null"));
		return;
	}

	if (!IsValid(FPSChildActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("ChildActor is pending kill"));
		return;
	}

	AWeapon_Base* FPSWeapon_Base = Cast<AWeapon_Base>(FPSChildActor);
	if (!IsValid(FPSWeapon_Base))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to cast ChildActor to AWeapon_Base"));
		return;
	}


	USkeletalMeshComponent* FPSWeaponMesh = FPSWeapon_Base->GetSkeletalMeshComponent();
	if (!IsValid(FPSWeaponMesh))
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponMesh is null"));
		return;
	}

	if (!FPSWeaponMesh->DoesSocketExist(FName("LHIK")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket LHIK does not exist"));
		return;
	}

	if (!FPSMesh->DoesSocketExist(FName("GrapSocket")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket GrapSocket does not exist"));
		return;
	}

	FTransform FPSWeaponSocketTransform = FPSWeaponMesh->GetSocketTransform(FName("LHIK"), RTS_World);
	FVector FPSOutPosition;
	FRotator FPSOutRotation;
	FPSMesh->TransformToBoneSpace(FName("hand_r"), FPSWeaponSocketTransform.GetLocation(), FRotator(FPSWeaponSocketTransform.GetRotation()), FPSOutPosition, FPSOutRotation);

	OutTransform.SetLocation(FPSOutPosition);
	OutTransform.SetRotation(FQuat(FPSOutRotation));
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
	if (FPSPlayerState && IsLocallyControlled() && FPSPlayerState->GetInventory()->GetInventory().Num() <= FPSPlayerState->GetInventory()->GetMaxItemCount()) {
		FPSPlayerState->GetInventory()->GetInventory().Add(Item);
		if (pickUp) {
			Server_DeleteItem(pickUp);
			EquipItem(0);
		}
	}
}

void Afps_cppCharacter::IF_GetAnimState_Implementation(EAnimStateEnum& AnimState)
{
	AnimState = CurrentAnimState;
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

