// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletHole.h"

// Sets default values
ABulletHole::ABulletHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletHole = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
}

// Called when the game starts or when spawned
void ABulletHole::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABulletHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

