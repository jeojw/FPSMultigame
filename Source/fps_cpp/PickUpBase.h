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

	UPROPERTY(EditAnywhere)
	UBoxComponent* WeaponBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* PickUpBox;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(EditAnywhere, Replicated)
	FDynamicInventoryItem bItem;

	UPROPERTY(EditAnywhere)
	TScriptInterface<IPlayerInterface> PlayerInterface;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void SetItem(FDynamicInventoryItem Item) { bItem = Item; }

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
