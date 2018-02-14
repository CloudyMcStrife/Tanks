// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tank.generated.h"

UCLASS()
class TANKS_API ATank : public APawn
{
	GENERATED_BODY()

	friend class ATankController;

public:
	// Sets default values for this pawn's properties
	ATank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void HandleMovement(float ForwardScale);
	void HandleRotation(float RotationScale);
	void HandleCanonRotation(FVector RotateTo);

	void ShootPrimaryProjectile();
	void ShootSecondaryProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Visual")
		class UStaticMeshComponent* UpperBody;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank|Visual")
		class UStaticMeshComponent* LowerBody;

	UPROPERTY(EditAnywhere, Category = "Tank|Controls")
		float MovementSpeed;

	UPROPERTY(EditAnywhere, Category = "Tank|Controls")
		float RotationSpeed;

	UPROPERTY(EditAnywhere, Category = "Tank|Controls")
		float CanonAngularVelocity;

	UPROPERTY(EditAnywhere, Category = "Tank|Projectiles")
		TSubclassOf<class AProjectile> PrimaryProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Tank|Projectiles")
		TSubclassOf<class AProjectile> SecondaryProjectileClass;

	void Shoot(TSubclassOf<class AProjectile> ProjectileClass);
};
