// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DynamicInventoryItem.h"
#include "Components/BoxComponent.h"
#include "PlayerInterface.h"
#include "PickUpBase.generated.h"

UCLASS()
class FPS_CPP_API APickUpBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* PickUpBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
	FDynamicInventoryItem Item;

	UPROPERTY(VisibleAnywhere)
	TScriptInterface<IPlayerInterface> PlayerInterface;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void SetItem(FDynamicInventoryItem _Item) { Item = _Item; }

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
