// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Shell_Base.generated.h"

UCLASS()
class FPS_CPP_API AShell_Base : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Shell;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	URotatingMovementComponent* RotatingMovement;
	
public:	
	// Sets default values for this actor's properties
	AShell_Base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UStaticMeshComponent* GetShell() const { return Shell; }
	void SetShell(UStaticMeshComponent* _Shell) { Shell = _Shell; }

	URotatingMovementComponent* GetRotatingMovement() const { return RotatingMovement; }

};
