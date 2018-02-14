// Fill out your copyright notice in the Description page of Project Settings.

#include "Target.h"

#include "Components/HealthComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Level/LevelGenerator.h"
#include "TimerManager.h"

#include "Tanks.h"

// Sets default values
ATarget::ATarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Target Mesh"));
	RootComponent = TargetMesh;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	HealthComponent->SetMaxHealth(1.0f);
	AddOwnedComponent(HealthComponent);

	Radius = 50.0f;

	LevelGenerator = nullptr;
	OtherTarget = nullptr;
}

// Called when the game starts or when spawned
void ATarget::BeginPlay()
{
	Super::BeginPlay();
	
	HealthComponent->OnZeroHealth.BindUObject(this, &ATarget::OnDestroy);
}

// Called every frame
void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATarget::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	TargetMesh->SetWorldScale3D(FVector(Radius * 0.02f));
}

void ATarget::OnDestroy()
{
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ATarget::Spawn, 2.0f, false);

	FActorHelper::EnableActor(*this, false);
}

void ATarget::Spawn()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);

	if (LevelGenerator)
	{
		bool bLocationIsValid = false;

		FVector NewLocation;
		while (!bLocationIsValid)
		{
			NewLocation = LevelGenerator->GetFreeLocation_Sphere(Radius);
			if (OtherTarget && !OtherTarget->bHidden)
			{
				FVector OtherLocation = OtherTarget->GetActorLocation();

				//We want the radius in unreal units to check the collision with the other target
				float RadiusUU = Radius;
				float OtherRadiusUU = OtherTarget->Radius;

				//Get the squared distance to avoid the root function
				float DistanceToOtherSquared = (OtherLocation - NewLocation).SizeSquared();

				//>= because they are allowed to touch each other
				bLocationIsValid = (((RadiusUU * RadiusUU) + (OtherRadiusUU * OtherRadiusUU)) <= DistanceToOtherSquared);
			}
			else
			{
				bLocationIsValid = true;
			}
		}

		NewLocation.Z = GetActorLocation().Z;

		SetActorLocation(NewLocation);
		SetActorTickEnabled(true);
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
	}
}