// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Target.generated.h"

UCLASS()
class TANKS_API ATarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	void OnDestroy();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		class UStaticMeshComponent* TargetMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		class UHealthComponent* HealthComponent;

protected:
	UPROPERTY(EditAnywhere, Category = "Level")
		class ALevelGenerator* LevelGenerator;

	UPROPERTY(EditAnywhere, Category = "Level")
		ATarget* OtherTarget;

	UPROPERTY(EditAnywhere, Category = "Level")
		float Radius;
private:
	FTimerHandle TimerHandle;
	void Spawn();
};
