// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicInventoryItem.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPS_CPP_API FDynamicInventoryItem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Bullets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Scope;
};
