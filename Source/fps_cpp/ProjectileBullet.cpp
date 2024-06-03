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

		Box->SetBoxExtent(FVector(3, 3, 3));

		FScriptDelegate OnHitDelegate, OnOverlapDelegate;
		OnHitDelegate.BindUFunction(this, "BoxComponentHit");
		OnOverlapDelegate.BindUFunction(this, "BoxComponentBeginOverlap");

		Box->OnComponentHit.AddUnique(OnHitDelegate);
		Box->OnComponentBeginOverlap.AddUnique(OnOverlapDelegate);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystemAsset(TEXT("/Game/MilitaryWeapSilver/FX/P_AssaultRifle_Tracer_01"));
	if (ParticleSystemAsset.Succeeded())
	{
		Tracer->SetTemplate(ParticleSystemAsset.Object);
	}
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 20000.0f;
	ProjectileMovement->MaxSpeed = 20000.0f;
	ProjectileMovement->bInitialVelocityInLocalSpace = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

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
	if (OtherActor)
	{
		if (OtherActor)
		{
			if (!OtherActor->ActorHasTag("Bullet") && OtherActor != Player)
			{
				if (!bHasExecuted)
				{
					bHasExecuted = true;

					// Check if the other actor is a static mesh or landscape, and destroy the bullet
					AStaticMeshActor* StaticActor = Cast<AStaticMeshActor>(OtherActor);
					ALandscape* Landscape = Cast<ALandscape>(OtherActor);
					if (StaticActor || Landscape)
					{
						Afps_cppCharacter* GetPlayer = Cast<Afps_cppCharacter>(Player);
						if (GetPlayer)
						{
							GetPlayer->ReceiveImpactProjectile(OtherActor, OtherComp, Hit.Location, Hit.Normal);

							this->Destroy();
						}
					}
				}
			}
		}
	}
}

void AProjectileBullet::BoxComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (!OtherActor->ActorHasTag(TEXT("Bullet")) && 
			OtherActor != Player && 
			(OtherActor->ActorHasTag(TEXT("Player")) || OtherActor->ActorHasTag(TEXT("Enemy"))))
		{
			if (!bHasExecuted)
			{
				bHasExecuted = true;
				Afps_cppCharacter* GetPlayer = Cast<Afps_cppCharacter>(Player);
				if (GetPlayer) 
				{
					GetPlayer->ReceiveImpactProjectile(OtherActor, OtherComp, SweepResult.Location, SweepResult.Normal);

					this->Destroy();
				}
				
			}
		}
	}
}
