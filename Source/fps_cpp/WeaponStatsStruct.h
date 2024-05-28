// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponStatsStruct.generated.h"

USTRUCT(BlueprintType)
struct FPS_CPP_API FWeaponStatsStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MagSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Range;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProceduralRecoil;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InputRecoil;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadTime;
};
