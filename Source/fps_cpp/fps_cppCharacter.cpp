// Copyright Epic Games, Inc. All Rights Reserved.

#include "fps_cppCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Engine/DataTable.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Math/RotationMatrix.h"
#include "Math/Transform.h"
#include "Kismet/GameplayStatics.h"
#include "BulletHole.h"
#include "Sound/SoundCue.h"
#include "PlayerInterfaceImplement.h"
#include "Particles/ParticleSystem.h"

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

	bIsAiming = false;
	bStopLeftHandIK = false;
	bIsAttacking = false;

	InventoryComponent = CreateDefaultSubobject<UInventory>(TEXT("InventoryComponent"));
	PlayerInterface = CreateDefaultSubobject<UPlayerInterfaceImplement>(TEXT("PlayerInterface"));
	if (PlayerInterface)
	{
		PlayerInterface->SetPlayer(this);
	}

	bAnimState = EAnimStateEnum::Hands;
}

void Afps_cppCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	EquiptItem();
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

void Afps_cppCharacter::Sprint()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		SprintServer(600.0f);
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
		SprintServer(100.0f);
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
	if (!bFireCooldownTimer.IsValid())
	{
		if (bIsAttacking)
		{
			if (InventoryComponent->GetInventory()[bCurrentItemSelection].Bullets >= 1)
			{
				
			}
		}
	}
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

void Afps_cppCharacter::DelayedFunction()
{
	bIsAttacking = true;
	
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
		StopLeftHandIKServer(true);
		AWeapon_Base* WB = Cast<AWeapon_Base>(WeaponBase);
		if (WB)
		{
			WB->PlayReloadAnimation(nullptr);
		}
		PlayAnimMontageServer(bCurrentReloadAnimation);
		GetWorld()->GetTimerManager().SetTimer(
			bFireCooldownTimer,
			this,
			&Afps_cppCharacter::DelayedFunction,
			bCurrentStats.ReloadTime,
			false
		);

		if (InventoryComponent && InventoryComponent->GetInventory().IsValidIndex(bCurrentItemSelection))
		{
			InventoryComponent->GetInventory()[bCurrentItemSelection].Bullets = bCurrentStats.MagSize;
			StopLeftHandIKServer(false);
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
			SpawnPickupActorServer(SpawnTransform, ESpawnActorCollisionHandlingMethod::Undefined, Item, bCurrentWeaponPickUpClass);

			InventoryComponent->GetInventory().RemoveAt(bCurrentItemSelection);
			bCurrentItemSelection = 0;
			EquiptItem();
		}
	}
}

void Afps_cppCharacter::EquiptItem()
{
	if (IsLocallyControlled()) {
		if (!InventoryComponent) {
			UE_LOG(LogTemp, Error, TEXT("Inventory is nullptr"));
			return;
		}

		if (!PlayerInterface) {
			UE_LOG(LogTemp, Error, TEXT("PlayerInterface is nullptr"));
			return;
		}

		int CurrentSelection = PlayerInterface->GetCurrentItemSelection();
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
				UE_LOG(LogTemp, Warning, TEXT("RowName: %s"), *RowNames[i].ToString());
				if (RowNames[i] == fname)
				{
					FItemDataTable* data = DT_ItemData->FindRow<FItemDataTable>(RowNames[i], RowNames[i].ToString());
					if (data)
					{
						SetWeaponClassServer(data->WeaponClass);
						SetStatsToServer(data->Stats);
						SetAnimStateServer(data->AnimState);
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
		SpawnedBulletActor->GetProjectileMovment()->Velocity *= 1;
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
		SpawnedBulletActor->GetProjectileMovment()->Velocity *= 1;
	}
}

void Afps_cppCharacter::ReceiveImpactProjectile(AActor* actor, UActorComponent* comp, FVector Loc, FVector Normal)
{
	FTransform tmpTransform = FTransform(FRotationMatrix::MakeFromX(Normal).Rotator(), Loc, FVector(1, 1, 1));
	if (actor) 
	{
		UGameplayStatics::ApplyDamage(actor, bCurrentStats.Damage, nullptr, this, nullptr);
		SpawnBulletHole(tmpTransform);
		AActor* HitActor = actor;
		if (actor->ActorHasTag("Metal"))
		{
			static ConstructorHelpers::FObjectFinder<USoundCue> SoundCueFinder(TEXT("/Game/MilitaryWeapSilver/Sound/Rifle/Cues/Rifle_ImpactSurface_Cue"));
			static ConstructorHelpers::FObjectFinder<UParticleSystem> EmitterTemplateFinder(TEXT("/Game/MilitaryWeapSilver/FX/P_Impact_Metal_Large_01"));
			if (EmitterTemplateFinder.Succeeded())
			{
				UParticleSystem* EmitterTemplate = EmitterTemplateFinder.Object;
				SpawnEmitterAtLocationServer(EmitterTemplate, Loc, FRotator(0, 0, 0), FVector(1, 1, 1));
				if (SoundCueFinder.Succeeded())
				{
					USoundCue* SoundCue = SoundCueFinder.Object;
					PlaySoundAtLocationServer(Loc, SoundCue);
				}
			}
		}
		else if (actor->ActorHasTag("Flesh"))
		{
			static ConstructorHelpers::FObjectFinder<USoundCue> SoundCueFinder(TEXT("/Game/MilitaryWeapSilver/Sound/Rifle/Cues/Rifle_ImpactSurface_Cue"));
			static ConstructorHelpers::FObjectFinder<UParticleSystem> EmitterTemplateFinder(TEXT("/Game/MilitaryWeapSilver/FX/P_Impact_Metal_Large_01"));
			if (EmitterTemplateFinder.Succeeded())
			{
				UParticleSystem* EmitterTemplate = EmitterTemplateFinder.Object;
				SpawnEmitterAtLocationServer(EmitterTemplate, Loc, FRotator(0, 0, 0), FVector(1, 1, 1));
				if (SoundCueFinder.Succeeded())
				{
					USoundCue* SoundCue = SoundCueFinder.Object;
					PlaySoundAtLocationServer(Loc, SoundCue);

					FVector Start = Loc;
					FVector End = FollowCamera->GetForwardVector() * 1500.0f + Loc;
					FCollisionQueryParams TraceParams(FName(TEXT("HitTrace")), true, this);
					TraceParams.AddIgnoredActor(HitActor);
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
						AActor* HitActor2 = HitResult.GetActor();
						FTransform tmpTransform2 = FTransform(FRotationMatrix::MakeFromX(Normal).Rotator(), HitActor2->GetActorLocation(), FVector(1, 1, 1));
						SpawnActorToServer(ABulletHole::StaticClass(), tmpTransform2, ESpawnActorCollisionHandlingMethod::Undefined);
					}
				}
			}
		}
		else {
			static ConstructorHelpers::FObjectFinder<USoundCue> SoundCueFinder(TEXT("/Game/MilitaryWeapSilver/Sound/Rifle/Cues/Rifle_ImpactSurface_Cue"));
			static ConstructorHelpers::FObjectFinder<UParticleSystem> EmitterTemplateFinder(TEXT("/Game/MilitaryWeapSilver/FX/P_Impact_Metal_Large_01"));
			if (EmitterTemplateFinder.Succeeded())
			{
				UParticleSystem* EmitterTemplate = EmitterTemplateFinder.Object;
				SpawnEmitterAtLocationServer(EmitterTemplate, Loc, FRotator(0, 0, 0), FVector(1, 1, 1));
				if (SoundCueFinder.Succeeded())
				{
					USoundCue* SoundCue = SoundCueFinder.Object;
					PlaySoundAtLocationServer(Loc, SoundCue);
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

void Afps_cppCharacter::PlaySoundAtLocationServer_Implementation(FVector Location, USoundBase* Sound)
{
	PlaySoundAtLocationMulticast_Implementation(Location, Sound);
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
	SpawnEmitterAtLocationMulticast_Implementation(EmitterTemplate, Location, Rotation, Scale);
}

void Afps_cppCharacter::SetWeaponClass(TSubclassOf<AActor> WBase)
{
	if (HasAuthority())
	{
		// 서버 권한이 있는 경우 서버 함수를 호출
		SetWeaponClassMulticast(WBase);
	}
	else
	{
		// 클라이언트인 경우 서버 함수 호출을 요청
		SetWeaponClassServer(WBase);
	}
}

void Afps_cppCharacter::SpawnActorToServer_Implementation(TSubclassOf<AActor> Class, FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride)
{
	UWorld* World = GetWorld();
	if (World && *Class)
	{
		// Actor를 Spawn합니다.
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = CollisionHandlingOverride;
		SpawnParams.Instigator = GetInstigator();

		AActor* SpawnedActor = World->SpawnActor<AActor>(Class, SpawnTransform, SpawnParams);
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
	if (WBase)
	{
		bCurrentWeaponClass = WBase;
		SetWeaponClassMulticast(WBase);
	}
}

bool Afps_cppCharacter::SetWeaponClassServer_Validate(TSubclassOf<AActor> WBase)
{
	return true;
}

void Afps_cppCharacter::StopLeftHandIKServer_Implementation(bool StopLeftHandIK)
{
	bStopLeftHandIK = StopLeftHandIK;
}

void Afps_cppCharacter::SetStatsToServer_Implementation(FWeaponStatsStruct CurrentStats)
{
	bCurrentStats = CurrentStats;
}

void Afps_cppCharacter::OnRep_AnimState() {
	OnAnimStateChanged.Broadcast();
}

void Afps_cppCharacter::SetAnimStateServer_Implementation(EAnimStateEnum AnimState)
{
	bAnimState = AnimState;
	OnRep_AnimState();
}

bool Afps_cppCharacter::SetAnimStateServer_Validate(EAnimStateEnum AnimState)
{
	return true;
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
	if (HasAuthority())
	{
		PlayAnimMontageMulticast_Implementation(AnimMontage);
	}
}

bool Afps_cppCharacter::PlayAnimMontageServer_Validate(UAnimMontage* AnimMontage)
{
	return true;
}

void Afps_cppCharacter::SpawnPickupActorServer_Implementation(FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, FDynamicInventoryItem Item, TSubclassOf<class APickUpBase> Class)
{
	if (HasAuthority())
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
}

bool Afps_cppCharacter::SpawnPickupActorServer_Validate(FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, FDynamicInventoryItem Item, TSubclassOf<class APickUpBase> Class)
{
	return true;
}

void Afps_cppCharacter::SpawnBulletHole_Implementation(FTransform SpawnTransform)
{
	if (HasAuthority())
	{
		if (GetWorld())
		{
			ABulletHole* NewActor = GetWorld()->SpawnActor<ABulletHole>(ABulletHole::StaticClass(), SpawnTransform);
		}
	}
	
}

bool Afps_cppCharacter::SpawnBulletHole_Validate(FTransform SpawnTransform)
{
	return true;
}

void Afps_cppCharacter::ApplyDamageServer_Implementation(AActor* DamageActor, float BaseDamage, AActor* DamageCauser)
{
	if (HasAuthority())
	{
		UGameplayStatics::ApplyDamage(DamageActor, BaseDamage, nullptr, DamageCauser, nullptr);
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