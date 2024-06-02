// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunInterface.h"
#include "Weapon_Base.generated.h"

UCLASS()
class FPS_CPP_API AWeapon_Base : public AActor, public IGunInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* AimOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* AimOffset_RedDot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform bSocketTransform;
	
public:	
	// Sets default values for this actor's properties
	AWeapon_Base();

	USkeletalMeshComponent* GetSkeletalMeshComponent() const { return SkeletalMesh; }
	USceneComponent* GetAimOffset() const { return AimOffset; }
	USceneComponent* GetAimOffset_RedDot() const { return AimOffset_RedDot; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	USkeletalMeshComponent* GetMesh() const { return SkeletalMesh; }
	void SetMesh(USkeletalMeshComponent* _mesh) { SkeletalMesh = _mesh; }

	virtual void GetShellTransform_Implementation(FTransform& T) override;

	UFUNCTION(BlueprintCallable)
	void PlayReloadAnimation(UAnimSequence* ReloadAnimation);
};
