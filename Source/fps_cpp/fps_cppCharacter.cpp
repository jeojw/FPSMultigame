// Copyright Epic Games, Inc. All Rights Reserved.

#include "fps_cppCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PlayerInterface.h"

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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	bIsAttacking = false;

	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));
}

void Afps_cppCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

void Afps_cppCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//////////////////////////////////////////////////////////////////////////
// Input

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
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &Afps_cppCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &Afps_cppCharacter::Look);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &Afps_cppCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &Afps_cppCharacter::StopSprint);

		// Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &Afps_cppCharacter::Fire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &Afps_cppCharacter::StopFire);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &Afps_cppCharacter::Aiming);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &Afps_cppCharacter::StopAiming);

		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &Afps_cppCharacter::Reload);

		EnhancedInputComponent->BindAction(DropItemAction, ETriggerEvent::Started, this, &Afps_cppCharacter::DropItem);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
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

void Afps_cppCharacter::DeleteItemServer_Implementation(AActor* DeleteItem)
{
	if (GetLocalRole() < ROLE_Authority) {
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

		if (UPlayerInterfaceImplement* playerInterface = Cast<UPlayerInterfaceImplement>(PlayerController->GetCharacter())) {
			playerInterface->Server_DeleteItem(DeleteItem);
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

void Afps_cppCharacter::Sprint()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		SprintServer(600.0f); // 서버로 Sprint 함수를 호출합니다.
	}
	else
	{
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		}
	}
}

void Afps_cppCharacter::StopSprint()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		SprintServer(100.0f); // 서버로 Sprint 함수를 호출합니다.
	}
	else
	{
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = 100.0f;
		}
	}
}

void Afps_cppCharacter::Fire()
{
	bIsAttacking = true;

	/*switch (WeaponType) 
	{
	case ItemType::Pistol:
		PistolFire();
		break;
		
	case ItemType::Rifle:
		RifleFire();
		break;

	default:
		break;
	}*/
}

void Afps_cppCharacter::RifleFire()
{

}

void Afps_cppCharacter::PistolFire()
{

}


void Afps_cppCharacter::StopFire()
{
	bIsAttacking = false;
}

void Afps_cppCharacter::Aiming()
{
	bIsAiming = true;
}

void Afps_cppCharacter::StopAiming()
{
	bIsAiming = false;
}

void Afps_cppCharacter::Reload()
{
	bIsAiming = false;
}

void Afps_cppCharacter::DropItem()
{
	bIsAiming = false;
}

void Afps_cppCharacter::EquiptItem()
{
	/*if (IsLocallyControlled()) {
		if (Inventory->GetInventory().IsValidIndex(PlayerInterface->GetCurrentItemSelection())) {

		}
		else {

		}
	}*/
}