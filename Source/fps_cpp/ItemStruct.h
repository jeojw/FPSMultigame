// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemTypeEnum.h"
#include "Weapon_Base.h"
#include "WeaponStatsStruct.h"
#include "AnimStateEnum.h"
#include "ItemStruct.generated.h"

USTRUCT(BlueprintType)
struct FPS_CPP_API FItemStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemTypeEnum Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> WeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponStatsStruct Stats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ReloadAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAnimStateEnum AnimState;
};