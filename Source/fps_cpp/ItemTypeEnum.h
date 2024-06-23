// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemTypeEnum.generated.h"

UENUM(BlueprintType)
enum class EItemTypeEnum : uint8
{
	Rifle UMETA(DisplayName = "Rifle"),
	Pistol UMETA(DisplayName = "Pistol"),
	Knife UMETA(DisplayName = "Knife")
};
