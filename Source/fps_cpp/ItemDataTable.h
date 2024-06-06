// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemTypeEnum.h"
#include "Weapon_Base.h"
#include "PickUpBase.h"
#include "WeaponStatsStruct.h"
#include "AnimStateEnum.h"
#include "PaperSprite.h"
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
    UPaperSprite* icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemTypeEnum Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AWeapon_Base> WeaponClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FWeaponStatsStruct Stats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* ReloadAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<APickUpBase> PickupClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAnimStateEnum AnimState;
};