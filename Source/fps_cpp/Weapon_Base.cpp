// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Base.h"

// Sets default values
AWeapon_Base::AWeapon_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/AR4/SK_AR4"));
	if (SkeletalMeshAsset.Succeeded())
	{
		SkeletalMesh->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load SkeletalMesh asset!"));
	}

	AimOffset = CreateDefaultSubobject<USceneComponent>(TEXT("AimOffset"));
	AimOffset->SetupAttachment(SkeletalMesh);

	AimOffset_RedDot = CreateDefaultSubobject<USceneComponent>(TEXT("AimOffset_RedDot"));
	AimOffset_RedDot->SetupAttachment(SkeletalMesh);
}

// Called when the game starts or when spawned
void AWeapon_Base::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeapon_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon_Base::PlayReloadAnimation(UAnimSequence* ReloadAnimation)
{
	if (SkeletalMesh && ReloadAnimation)
	{
		SkeletalMesh->PlayAnimation(ReloadAnimation, false); // false means not looping
	}
}

