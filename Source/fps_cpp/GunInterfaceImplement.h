// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunInterface.h"
#include "Weapon_Base.h"
#include "GunInterfaceImplement.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API UGunInterfaceImplement : public UObject, public IGunInterface
{
	GENERATED_BODY()

	TWeakObjectPtr<class AWeapon_Base> WeaponMesh;

public:
	virtual void GetShellTransform_Implementation(FTransform& T) override;
};
