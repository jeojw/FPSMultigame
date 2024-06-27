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

UCLASS()
class FPS_CPP_API UEnumUtils : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Enum")
    static FString GetAnimStateEnumAsString(EAnimStateEnum EnumValue)
    {
        const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EAnimStateEnum"), true);
        if (!EnumPtr)
        {
            return FString("Invalid");
        }
        return EnumPtr->GetNameStringByValue(static_cast<int32>(EnumValue));
    }
};