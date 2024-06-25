// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Base_Knife.h"

AWeapon_Base_Knife::AWeapon_Base_Knife() : AWeapon_Base()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/MilitaryWeapSilver/Weapons/Knife_A"));
	if (SkeletalMeshAsset.Succeeded())
	{
		SkeletalMesh->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}
}