// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwarmOrigin.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	APPROACH,
	ROTATE
};

struct FDefaultSwarmValues
{
	float TargetWeight;
	float SwarmSpeed;
	float SeperationWeight;
};

UCLASS()
class TANKS_API ASwarmOrigin : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwarmOrigin();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Swarm")
		class USwarmInstancedComponent* SwarmInstances;

	void Approach();

	UFUNCTION(BlueprintCallable, Category = "Swarm|Behavior")
		void Attack(EAttackType AttackType);

	void LerpToDefaultValues();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm|Behavior")
		AActor* Target;

protected:
	float LerpTime = 2.0f;
	FTimerHandle Timer;


	FDefaultSwarmValues Values;
};
