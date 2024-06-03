// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Base_Pistol.h"

AWeapon_Base_Pistol::AWeapon_Base_Pistol() : Super()
{
	static ConstructorHelpers::FObjectFinder<UAnimSequence> ShotSequenceFinder(TEXT("/Game/MilitaryWeapSilver/Weapons/Animations/Fire_Pistol_W"));
	if (ShotSequenceFinder.Succeeded())
	{
		ShotSequence = ShotSequenceFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> ReloadSequenceFinder(TEXT("/Game/MilitaryWeapSilver/Weapons/Animations/Reload_Pistol_W"));
	if (ReloadSequenceFinder.Succeeded())
	{
		ReloadSequence = ReloadSequenceFinder.Object;
	}
}

void AWeapon_Base_Pistol::BeginPlay()
{
	Super::BeginPlay();

	if (!ShotSequence)
	{
		ShotSequence = Cast<UAnimSequence>(StaticLoadObject(UAnimSequence::StaticClass(), nullptr, TEXT("/Game/MilitaryWeapSilver/Weapons/Animations/Fire_Rifle_W")));
	}
	if (!ReloadSequence)
	{
		ReloadSequence = Cast<UAnimSequence>(StaticLoadObject(UAnimSequence::StaticClass(), nullptr, TEXT("/Game/MilitaryWeapSilver/Weapons/Animations/Prone_Reload_Rifle_W")));
	}
}

void AWeapon_Base_Pistol::PlayShotSequence()
{
	Super::PlayShotSequence();
	if (ShotSequence && SkeletalMesh)
	{
		SkeletalMesh->PlayAnimation(ShotSequence, false);
	}
}

void AWeapon_Base_Pistol::PlayReloadSequence()
{
	Super::PlayReloadSequence();
	if (ShotSequence && SkeletalMesh)
	{
		SkeletalMesh->PlayAnimation(ReloadSequence, false);
	}
}