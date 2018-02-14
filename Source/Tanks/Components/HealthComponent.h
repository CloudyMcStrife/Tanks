// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DELEGATE(FOnZeroHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TANKS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

public:	
	// Called every frame
	float GetHealth();

	void AddDamage(float Damage);

	FOnZeroHealth OnZeroHealth;

	void SetMaxHealth(float Value);

protected:
	UPROPERTY(EditAnywhere)
		float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Health")
		float CurrentHealth;
	
};
