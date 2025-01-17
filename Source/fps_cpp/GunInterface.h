// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GunInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGunInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPS_CPP_API IGunInterface
{
	GENERATED_BODY()
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetShellTransform(FTransform& T);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetAimSocketTransform(FTransform& T);
};

