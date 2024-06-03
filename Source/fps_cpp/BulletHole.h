// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/DecalComponent.h" 
#include "Components/TimelineComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "BulletHole.generated.h"

UCLASS()
class FPS_CPP_API ABulletHole : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UDecalComponent* BulletHole;

	UPROPERTY(EditAnywhere, Category = "Decal")
	UMaterialInterface* BulletHoleMaterial;

	UPROPERTY(EditAnywhere)
	UCurveFloat* TimelineCurve;

	UPROPERTY(EditAnywhere)
	UTimelineComponent* Timeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	UPROPERTY()
	FTimeline MyTimeline;

	UFUNCTION()
	void TimelineUpdate(float Value);
	
public:	
	// Sets default values for this actor's properties
	ABulletHole();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
