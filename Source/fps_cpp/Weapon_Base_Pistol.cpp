// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Base_Pistol.h"
#include "Kismet/GameplayStatics.h"

AWeapon_Base_Pistol::AWeapon_Base_Pistol() : AWeapon_Base()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/MilitaryWeapSilver/Weapons/Pistols_A"));
	if (SkeletalMeshAsset.Succeeded())
	{
		SkeletalMesh->SetSkeletalMesh(SkeletalMeshAsset.Object);
		SkeletalMesh->SetSkeletalMesh(SkeletalMeshAsset.Object);
		AimSocketTransform = SkeletalMesh->GetSocketTransform(FName("AimSocket"));
	}

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