// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "SwarmInstancedComponent.generated.h"

struct FSwarmInstance
{
	int32 Index;

	FVector Location;
	FVector RotationDirection;
	float RotationAngle;
	FVector Velocity;

	TArray<FSwarmInstance*> FriendInstances;
};

UENUM(BlueprintType)
enum class EFormation : uint8
{
	SPHERE,
	ROTATION,
	BEE
};

/**
 * 
 */
UCLASS()
class TANKS_API USwarmInstancedComponent : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	USwarmInstancedComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//virtual void TickComponent(float DeltaTime)
	void MergeSwarm(USwarmInstancedComponent* OtherSwarm);

	void SetSwarmSpeed(float NewSpeed);
	void SetTargetWeight(float NewWeight);
	void SetSeperationWeight(float NewWeight);

	float GetSwarmSpeed();
	float GetTargetWeight();
	float GetSeperationWeight();
	//void SetSwarmSpeed(float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = "Swarm|Properties")
		void SetProperties(float NewSpeed, float NewTargetWeight, float NewSeperationWeight);

	UFUNCTION(BlueprintCallable, Category = "Swarm|Formation")
		void SetFormation(EFormation Formation);

protected:
	void RotateInstanceAroundTarget(FSwarmInstance* Instance);
	void FlockInstance(FSwarmInstance* Instance, const float& InstanceSpeed, const FVector& InstanceLocation);
	FVector SeperateInstance(FSwarmInstance* Instance, const FVector& InstanceLocation);
	FVector AlignInstance(FSwarmInstance* Instance, const FVector& InstanceLocation);
	FVector CohereInstance(FSwarmInstance* Instance, const FVector& InstanceLocation);

	UPROPERTY(EditAnywhere, Category = "Target")
		AActor* Target;

	UPROPERTY(EditAnywhere, Category = "Swarm|Controls|Weights")
		float VelocityWeight;

	UPROPERTY(EditAnywhere, Category = "Swarm|Controls|Weights")
		float TargetWeight;

	UPROPERTY(EditAnywhere, Category = "Swarm|Controls")
		float SwarmSpeed;

	UPROPERTY(EditAnywhere, Category = "Swarm|Controls|Radii")
		float InstanceRadius;

	UPROPERTY(EditAnywhere, Category = "Swarm|Controls|Radii")
		float SeperationRadius;

	UPROPERTY(EditAnywhere, Category = "Swarm|Controls|Weights")
		float SeperationWeight;
	UPROPERTY(EditAnywhere, Category = "Swarm|Controls|Weights")
		float AlignWeight;
	UPROPERTY(EditAnywhere, Category = "Swarm|Controls|Weights")
		float CohereWeight;

	UPROPERTY(EditAnywhere, Category = "Swarm|Controls")
		bool bRotateAroundTarget;

	UPROPERTY(EditAnywhere, Category = "Swarm|Controls")
		float MagnetScale;

		
	bool IsCollision(FVector FirstLocation, FVector SecondLocation);

	TArray<FSwarmInstance*> Instances;
};
