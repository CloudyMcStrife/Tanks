// Fill out your copyright notice in the Description page of Project Settings.

#include "Tank.h"

#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"

#include "Projectiles/Projectile.h"

#include "Helper/ActorPool.h"


// Sets default values
ATank::ATank()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LowerBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LowerBody"));
	UpperBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UpperBody"));

	RootComponent = LowerBody;

	UpperBody->SetupAttachment(LowerBody);

	MovementSpeed = 100.0f;
	RotationSpeed = 50.0f;

	CanonAngularVelocity = 200.0f;
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATank::EndPlay(const EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

	ActorPool::DeleteInstance();
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Forward", this, &ATank::HandleMovement);
	InputComponent->BindAxis("RotateRight", this, &ATank::HandleRotation);

	InputComponent->BindAction("Shoot1", IE_Pressed, this, &ATank::ShootPrimaryProjectile);
	InputComponent->BindAction("Shoot2", IE_Pressed, this, &ATank::ShootSecondaryProjectile);
}

void ATank::HandleMovement(float ForwardScale)
{
	if (FMath::Abs(ForwardScale) >= 0.01f)
	{
		FVector NewLocation = GetActorLocation() + LowerBody->GetForwardVector() * ForwardScale * MovementSpeed * FApp::GetDeltaTime();

		SetActorLocation(NewLocation, true);
	}
}

void ATank::HandleRotation(float RotationScale)
{
	float Angle = RotationScale * RotationSpeed * FApp::GetDeltaTime();

	LowerBody->AddWorldRotation(FRotator(0, Angle, 0), true);
}

void ATank::HandleCanonRotation(FVector RotateTo)
{
	FVector RotateToDir = (RotateTo - GetActorLocation()).GetSafeNormal();

	FVector Forward = UpperBody->GetForwardVector();

	//Get Angle between RotateToDir and Forwardvector of the Canon (x2*x1) + (y2 * y1)
	float a = Forward.X * RotateToDir.X + Forward.Y * RotateToDir.Y;
	float b = Forward.X * RotateToDir.Y - Forward.Y * RotateToDir.X;

	float Angle = FMath::Atan2(b, a);

	float AngularVelocity = CanonAngularVelocity * FApp::GetDeltaTime();

	if (FMath::Abs(Angle) < 0.01f)
	{
		AngularVelocity = 0.0f;
	}
	else
	{
		AngularVelocity *= (Angle < 0.0f) ? -1.0f : 1.0f;
	}
	
	UpperBody->AddWorldRotation(FRotator(0, AngularVelocity, 0));
}

void ATank::ShootPrimaryProjectile()
{
	Shoot(PrimaryProjectileClass);
}

void ATank::ShootSecondaryProjectile()
{
	Shoot(SecondaryProjectileClass);
}

void ATank::Shoot(TSubclassOf<class AProjectile> ProjectileClass)
{
	if (AProjectile* Projectile = Cast<AProjectile>(ActorPool::GetInstance()->GetResource(GetWorld(), ProjectileClass).Get()))
	{
		FVector CanonOutput = UpperBody->GetSocketLocation("CanonOuput");

		//TODO: Implement function HandleActorHit in ATank or let Projectile handle the damaging
		Projectile->OnDestruction.BindLambda([](AProjectile* Projectile)
		{
			ActorPool::GetInstance()->ReturnResource(Projectile);
		});

		Projectile->Launch(CanonOutput, UpperBody->GetForwardVector().ToOrientationRotator(), ((ProjectileClass == SecondaryProjectileClass) ? 10.0f : 3.0f));
	}
}
