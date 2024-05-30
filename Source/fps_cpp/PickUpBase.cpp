// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APickUpBase::APickUpBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(RootComponent);

	PickUpBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PickUpBox"));
	PickUpBox->SetupAttachment(WeaponBox);

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetupAttachment(WeaponBox);

	bReplicates = true;
}

// Called when the game starts or when spawned
void APickUpBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APickUpBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickUpBase::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		PlayerInterface->IF_AddItemToInventory(bItem, this);
	}
}

void APickUpBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickUpBase, bItem);
}

