// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Inventory.Add(FDynamicInventoryItem(1, 30, 0));
	Inventory.Add(FDynamicInventoryItem(2, 8, 0));
	Inventory.Add(FDynamicInventoryItem(3, 0, 0));

	MaxItemCount = 5;
	// ...
}


// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int UInventory::GetCurID(int current) const
{
	return Inventory[current].ID;
}

int UInventory::GetCurBullet(int current) const
{
	return Inventory[current].Bullets;
}

void UInventory::ReduceBullet(int current)
{
	Inventory[current].Bullets -= 1;
}
void UInventory::ReloadBullet(int current, int maxSize)
{
	Inventory[current].Bullets = maxSize;
}

