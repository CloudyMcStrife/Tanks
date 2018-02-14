// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
}

void UHealthComponent::SetMaxHealth(float Value)
{
	MaxHealth = Value;
	
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
}

float UHealthComponent::GetHealth()
{
	return CurrentHealth;
}

void UHealthComponent::AddDamage(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	
	if (CurrentHealth <= 0.0001f)
	{
		OnZeroHealth.ExecuteIfBound();
	}
}
