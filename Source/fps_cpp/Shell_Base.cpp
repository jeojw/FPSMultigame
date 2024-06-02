// Fill out your copyright notice in the Description page of Project Settings.


#include "Shell_Base.h"

// Sets default values
AShell_Base::AShell_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Shell = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shell"));
	Shell->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/Ammunition/SM_Shell_545x39_Empty"));
	if (StaticMeshAsset.Succeeded())
	{
		Shell->SetStaticMesh(StaticMeshAsset.Object);
	}

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(-50.0f, 0, 50.0f);
	RotatingMovement->bRotationInLocalSpace = true;
}

// Called when the game starts or when spawned
void AShell_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShell_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

