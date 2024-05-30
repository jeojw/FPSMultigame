// Fill out your copyright notice in the Description page of Project Settings.


#include "GunInterfaceImplement.h"
void UGunInterfaceImplement::GetShellTransform_Implementation(FTransform& T)
{
	FTransform SocketTransform = WeaponMesh->GetSkeletalMeshComponent()->GetSocketTransform(FName("ShellSocket"));
	T = SocketTransform;
}