// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Base_M4.h"

AWeapon_Base_M4::AWeapon_Base_M4() : AWeapon_Base()
{
	// Using static ConstructorHelpers to find animations during BeginPlay
	static ConstructorHelpers::FObjectFinder<UAnimSequence> ShotSequenceFinder(TEXT("/Game/MilitaryWeapSilver/Weapons/Animations/Fire_Rifle_W"));
	if (ShotSequenceFinder.Succeeded())
	{
		ShotSequence = ShotSequenceFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> ReloadSequenceFinder(TEXT("/Game/MilitaryWeapSilver/Weapons/Animations/Prone_Reload_Rifle_W"));
	if (ReloadSequenceFinder.Succeeded())
	{
		ReloadSequence = ReloadSequenceFinder.Object;
	}
}