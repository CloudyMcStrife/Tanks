// Fill out your copyright notice in the Description page of Project Settings.

#include "TankController.h"

#include "Tank.h"

#include "Runtime/Engine/Public/DrawDebugHelpers.h"

void ATankController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
}


void ATankController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ATank* Tank = Cast<ATank>(GetPawn()))
	{
		FVector WorldLocation, WorldDirection;
		DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

		FVector TankLocation = Tank->GetActorLocation();

		FVector IntersectionPoint = FMath::LinePlaneIntersection(WorldLocation, WorldLocation + WorldDirection * 10000, TankLocation, Tank->GetActorUpVector());

		DrawDebugBox(GetWorld(), IntersectionPoint, FVector(20), FColor::Blue, false, 0.1f, 0, 10.0f);

		Tank->HandleCanonRotation(IntersectionPoint);
	}
}
