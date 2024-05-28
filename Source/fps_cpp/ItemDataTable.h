// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CompositeDataTable.h"
#include "ItemStruct.h"
#include "ItemDataTable.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FItemDataTable : public FTableRowBase
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemTypeEnum Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AWeapon_Base> WeaponClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FWeaponStatsStruct Stats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* ReloadAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimInstance* AnimState;
};