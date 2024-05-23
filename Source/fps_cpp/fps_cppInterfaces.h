// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "fps_cppInterfaces.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class Ufps_cppInterfaces : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPS_CPP_API Ifps_cppInterfaces
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interface)
	FTransform GetLeftHandSocketTransform() const;
};
