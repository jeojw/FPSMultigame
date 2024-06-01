// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Engine/StaticMeshActor.h"
#include "Runtime/Landscape/Classes/Landscape.h"
#include "fps_cppCharacter.h"

// Sets default values
AProjectileBullet::AProjectileBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);

	Tracer = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletTracer"));
	Tracer->SetupAttachment(Box);

	if (Box)
	{
		// Set collision settings if needed
		Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Box->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

		Box->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
		Box->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);
		Box->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		Box->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		Box->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
		Box->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);
		Box->SetCollisionResponseToChannel(ECC_Destructible, ECR_Overlap);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystemAsset(TEXT("/Game/MilitaryWeapSilver/FX/P_AssaultRifle_Tracer_01"));
	if (ParticleSystemAsset.Succeeded())
	{
		Tracer->SetTemplate(ParticleSystemAsset.Object);
	}
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 40000.0f;
	ProjectileMovement->MaxSpeed = 40000.0f;
	ProjectileMovement->bInitialVelocityInLocalSpace = true;

	bHasExecuted = false;
}

// Called when the game starts or when spawned
void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectileBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileBullet::BoxComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor) {
		Afps_cppCharacter* Player = Cast<Afps_cppCharacter>(OtherActor);
		if (!OtherActor->ActorHasTag("Bullet") && !OtherActor->ActorHasTag("Player"))
		{
			if (!bHasExecuted)
			{
				bHasExecuted = true;
				AStaticMeshActor* staticActor = Cast<AStaticMeshActor>(OtherActor);
				if (staticActor)
				{
					Player->IF_ReceiveProjectileImpact(OtherActor, OtherComp, Hit.Location, Hit.Normal);
					OtherActor->Destroy();
				}
				else
				{
					ALandscape* landScape = Cast<ALandscape>(OtherActor);
					if (landScape)
					{
						Player->IF_ReceiveProjectileImpact(OtherActor, OtherComp, Hit.Location, Hit.Normal);
						OtherActor->Destroy();
					}

				}
			}
		}
	}
}
