// Fill out your copyright notice in the Description page of Project Settings.

#include "HomingMissle.h"

#include "Enemies/Target.h"
#include "Engine/World.h"

AHomingMissle::AHomingMissle()
{
	PrimaryActorTick.bCanEverTick = true;

	Speed = 100.0f;
	bIsLaunched = false;
	Time = 0.0f;

	AngularVelocity = 100.0f;
}

void AHomingMissle::Reset()
{
	Super::Reset();
	Target = nullptr;
}

FHitResult AHomingMissle::FlyBehavior(float DeltaTime)
{
	if (Target && !Target->bHidden)
	{
		FVector RotateTo = Target->GetActorLocation();
		FVector RotateToDir = (RotateTo - GetActorLocation()).GetSafeNormal();

		FVector Forward = GetActorForwardVector();

		float a = Forward.X * RotateToDir.X + Forward.Y * RotateToDir.Y;
		float b = Forward.X * RotateToDir.Y - Forward.Y * RotateToDir.X;

		float Angle = FMath::Atan2(b, a);

		float AngVelocity = AngularVelocity * DeltaTime;

		if (FMath::Abs(Angle) < 0.01f)
		{
			AngVelocity = 0.0f;
		}
		else
		{
			AngVelocity *= (Angle < 0.0f) ? -1.0f : 1.0f;
		}
		
		AddActorWorldRotation(FRotator(0, AngVelocity, 0));
	}
	else
	{
		Target = nullptr;

		TArray<AActor*> Actors;
		GetOverlappingActors(Actors);

		for (int i = 0; i < Actors.Num(); ++i)
		{
			if (Cast<ATarget>(Actors[i]))
			{
				Target = Actors[i];
				break;
			}
		}
	}

	return Super::FlyBehavior(DeltaTime);
}

