// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletHole.h"

// Sets default values
ABulletHole::ABulletHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletHole = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	BulletHole->SetupAttachment(RootComponent);
    BulletHole->SetWorldScale3D(FVector(0.3f, 0.05f, 0.05f));
    this->InitialLifeSpan = 10.0f;

    Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RecoilTimeline"));

    static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Game/Characters/Mannequins/Animations/BulletHoleCurve"));
    if (Curve.Succeeded())
    {
        TimelineCurve = Curve.Object;
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> DecalMaterial(TEXT("/Game/Megascans/Decals/Concrete_Patch_Oil_sdpqjfi/M_DynamicDecal"));
    if (DecalMaterial.Succeeded())
    {
        BulletHoleMaterial = DecalMaterial.Object;
        BulletHole->SetDecalMaterial(BulletHoleMaterial);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load bullet hole material"));
    }
}

// Called when the game starts or when spawned
void ABulletHole::BeginPlay()
{
	Super::BeginPlay();
	
    DynamicMaterialInstance = BulletHole->CreateDynamicMaterialInstance();

    if (TimelineCurve)
    {
        FOnTimelineFloat TimelineCallback;
        TimelineCallback.BindUFunction(this, FName("TimelineUpdate"));
        MyTimeline.AddInterpFloat(TimelineCurve, TimelineCallback);
        MyTimeline.SetLooping(false);
        MyTimeline.PlayFromStart();
    }
}

// Called every frame
void ABulletHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (MyTimeline.IsPlaying())
    {
        MyTimeline.TickTimeline(DeltaTime);
    }

}

void ABulletHole::TimelineUpdate(float Value)
{
    if (DynamicMaterialInstance)
    {
        DynamicMaterialInstance->SetScalarParameterValue(FName("emissive_boost"), Value);
    }
}

