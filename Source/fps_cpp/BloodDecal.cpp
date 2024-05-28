// Fill out your copyright notice in the Description page of Project Settings.


#include "BloodDecal.h"

// Sets default values
ABloodDecal::ABloodDecal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BloodDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
}

// Called when the game starts or when spawned
void ABloodDecal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABloodDecal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

