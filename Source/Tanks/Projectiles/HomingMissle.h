// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/Projectile.h"
#include "HomingMissle.generated.h"

/**
 * 
 */
UCLASS()
class TANKS_API AHomingMissle : public AProjectile
{
	GENERATED_BODY()
	
public:
	AHomingMissle();

	virtual FHitResult FlyBehavior(float DeltaTime) override;

	virtual void Reset() override;

	UPROPERTY(EditAnywhere)
	class AActor* Target;

	UPROPERTY(EditAnywhere, Category = "Projectile|Movement")
		float AngularVelocity;
};
