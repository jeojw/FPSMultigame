// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"

// Sets default values
AProjectileBullet::AProjectileBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);

	Tracer = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletTracer"));
	Tracer->SetupAttachment(Box);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

// Called when the game starts or when spawned
void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectileBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

