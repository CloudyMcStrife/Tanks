// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

DECLARE_DELEGATE_OneParam(FOnDestruction, class AProjectile*);
//DECLARE_DELEGATE_OneParam(FOnActorHit, AActor*, class AProjectile*);
//DECLARE_DELEGATE_OneParam(FOnTimeExpired, class AProjectile*);

UCLASS()
class TANKS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Launch(FVector FromLocation, FRotator StartRotation, float TimeTillExpiration = 3.0f);
	virtual FHitResult FlyBehavior(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Hit")
	void OnHit();

	virtual void Reset();

	void ExecuteHit(const FHitResult& HitResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Controls")
		float Speed;

	bool bIsLaunched;

	FOnDestruction OnDestruction;
	/*FOnActorHit OnActorHit;
	FOnTimeExpired OnTimeExpired;*/

	UPROPERTY(EditAnywhere, Category = "Projectile|Visuals")
		class UStaticMeshComponent* ProjectileMesh;

	float ExpirationTime;

protected:
	float Time;
};
