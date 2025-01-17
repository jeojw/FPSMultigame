// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/DecalComponent.h" 
#include "BloodDecal.generated.h"

UCLASS()
class FPS_CPP_API ABloodDecal : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UDecalComponent* BloodDecal;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* BloodMaterial;
	
	FTimerHandle TimerHandle;

	void DestroyBloodDecal();
	
public:	
	// Sets default values for this actor's properties
	ABloodDecal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
