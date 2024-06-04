// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon_Base.h"
#include "Weapon_Base_Pistol.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API AWeapon_Base_Pistol : public AWeapon_Base
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
public:
	AWeapon_Base_Pistol();
};
