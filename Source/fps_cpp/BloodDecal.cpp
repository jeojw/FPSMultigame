// Fill out your copyright notice in the Description page of Project Settings.


#include "BloodDecal.h"

// Sets default values
ABloodDecal::ABloodDecal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BloodDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));

	BloodDecal->SetRelativeScale3D(FVector(0.0475f, 0.3925f, 0.5225f));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DecalMaterial(TEXT("/Game/Megascans/Decals/High_Velocity_Blood_Spatter_sfjjcfzp/MI_High_Velocity_Blood_Spatter_sfjjcfzp_8K"));
	if (DecalMaterial.Succeeded())
	{
		BloodMaterial = DecalMaterial.Object;
		BloodDecal->SetDecalMaterial(BloodMaterial);
	}

	bReplicates = true;
}

// Called when the game starts or when spawned
void ABloodDecal::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABloodDecal::DestroyBloodDecal, 5.0f, false);
	
}

// Called every frame
void ABloodDecal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABloodDecal::DestroyBloodDecal()
{
	Destroy();
}

