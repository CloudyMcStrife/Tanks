// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"

#include "Components/HealthComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	RootComponent = ProjectileMesh;

	Speed = 500.0f;
	bIsLaunched = false;
	Time = 0.0f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsLaunched)
	{
		Time += DeltaTime;
		
		ExecuteHit(FlyBehavior(DeltaTime));

		//We let the Projectile fly for a specific time because its not guaranteed to hit anything. TODO: Make ProjectilePool
		if (Time >= ExpirationTime)
		{
			Reset();
			OnDestruction.ExecuteIfBound(this);
		}
	}
}

void AProjectile::Reset()
{
	Time = 0.0f;
	bIsLaunched = false;
}

void AProjectile::Launch(FVector FromLocation, FRotator StartRotation, float TimeTillExpiration)
{
	bIsLaunched = true;
	ExpirationTime = TimeTillExpiration;

	//No sweep because we initialize the actors location without wanting to get any hits
	SetActorLocationAndRotation(FromLocation, StartRotation);
}

FHitResult AProjectile::FlyBehavior(float DeltaTime)
{
	FVector NewLocation = GetActorLocation() + GetActorForwardVector() * DeltaTime * Speed;

	FHitResult Hit;
	//Let the Projectile sweep so we can trigger Hits from here
	SetActorLocation(NewLocation, true, &Hit);

	return Hit;
}

void AProjectile::ExecuteHit(const FHitResult& HitResult)
{
	if (AActor* Actor = HitResult.Actor.Get())
	{
		if (UHealthComponent* HealthComponent = Cast<UHealthComponent>(Actor->GetComponentByClass(UHealthComponent::StaticClass())))
		{
			HealthComponent->AddDamage(10.0f);
		}

		OnDestruction.ExecuteIfBound(this);
		OnHit();

		Reset();
	}
}

