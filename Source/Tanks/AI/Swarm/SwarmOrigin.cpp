// Fill out your copyright notice in the Description page of Project Settings.

#include "SwarmOrigin.h"

#include "EngineUtils.h"
#include "Components/SwarmInstancedComponent.h"

// Sets default values
ASwarmOrigin::ASwarmOrigin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SwarmInstances = CreateDefaultSubobject<USwarmInstancedComponent>(TEXT("Swarm Instances"));
	RootComponent = SwarmInstances;

	Target = nullptr;
}

// Called when the game starts or when spawned
void ASwarmOrigin::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<ASwarmOrigin> It(GetWorld()); It; ++It)
	{
		if (*It != this)
		{
			USwarmInstancedComponent* OtherInstances = Cast<USwarmInstancedComponent>(It->GetComponentByClass(USwarmInstancedComponent::StaticClass()));
			SwarmInstances->MergeSwarm(OtherInstances);
		}
	}
}

// Called every frame
void ASwarmOrigin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Target)
	{
		SetActorLocation(Target->GetActorLocation());
	}
}


#include "TimerManager.h"

void ASwarmOrigin::Approach()
{
	SwarmInstances->SetTargetWeight(100.0f);
	SwarmInstances->SetSeperationWeight(0.0f);
	SwarmInstances->SetSwarmSpeed(5000.0f);

	//GetWorldTimerManager().SetTimer(Timer, this, &ASwarmOrigin::LerpToDefaultValues, 0.02f, true, 0.0f);
}

void ASwarmOrigin::LerpToDefaultValues()
{
	float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(Timer);
	float EndTime = 2.0f;

	UE_LOG(LogTemp, Warning, TEXT("%f"), ElapsedTime);

	float Alpha = ElapsedTime / EndTime;
	
	float TargetWeight		= Values.TargetWeight		+ (100 - Values.TargetWeight)		* Alpha;
	float SwarmSpeed		= Values.SwarmSpeed			+ (10000.0f - Values.SwarmSpeed)			* Alpha;
	float SeperationWeight	= Values.SeperationWeight	+ (10.0f - Values.SeperationWeight)	* Alpha;

	SwarmInstances->SetTargetWeight(TargetWeight);
	SwarmInstances->SetSeperationWeight(SeperationWeight);
	SwarmInstances->SetSwarmSpeed(SwarmSpeed);

	if (ElapsedTime >= LerpTime)
	{
		GetWorldTimerManager().ClearTimer(Timer);
	}

}

void ASwarmOrigin::Attack(EAttackType AttackType)
{
	if (!SwarmInstances) return;

	Values.TargetWeight = SwarmInstances->GetTargetWeight();
	Values.SeperationWeight = SwarmInstances->GetSeperationWeight();
	Values.SwarmSpeed = SwarmInstances->GetSwarmSpeed();

	SwarmInstances->SetTargetWeight(0.0f);
	SwarmInstances->SetSeperationWeight(1000.0f);
	SwarmInstances->SetSwarmSpeed(2500.0f);

	GetWorldTimerManager().SetTimer(Timer, this, &ASwarmOrigin::Approach, 2.0f, false);
}