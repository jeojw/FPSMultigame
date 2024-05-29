// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimStateEnum.generated.h"

UENUM(BlueprintType)
enum class EAnimStateEnum : uint8
{
	Hands UMETA(DisplayName = "Hands"),
	Rifle UMETA(DisplayName = "Rifle"),
	Pistol UMETA(DisplayName = "Pistol"),
	Melee UMETA(DisplayName = "Melee"),
};