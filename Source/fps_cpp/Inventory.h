// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DynamicInventoryItem.h"
#include "Inventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_CPP_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventory();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDynamicInventoryItem> Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxItemCount;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetInventory(TArray<FDynamicInventoryItem> _Inventory) { Inventory = _Inventory; }
	TArray<FDynamicInventoryItem> GetInventory() const { return Inventory; }

	void SetMaxItemCount(int _max) { MaxItemCount = _max; }
	int GetMaxItemCount() const { return MaxItemCount; }
};
