// Fill out your copyright notice in the Description page of Project Settings.

#include "SwarmInstancedComponent.h"

USwarmInstancedComponent::USwarmInstancedComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SwarmSpeed = 100.0f;
	InstanceRadius = 200.0f;
	SeperationRadius = 100.0f;

	VelocityWeight = 10.0f;
	TargetWeight = 20.0f;
	SeperationWeight = 50.0f;
	AlignWeight = 50.0f;
	CohereWeight = 100.0f;

	MagnetScale = 10.0f;

	bRotateAroundTarget = false;
}

void USwarmInstancedComponent::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < 400; ++i)
	{
		FVector Location = FVector(FMath::RandRange(0.0f, 2000.0f), FMath::RandRange(0.0f, 2000.0f), FMath::RandRange(0.0f, 2000.0f));
		FRotator Rotation = (GetComponentLocation() - Location).ToOrientationRotator();

		AddInstanceWorldSpace(FTransform(Rotation, Location, FVector(1.0f)));
	}

	for (int i = 0; i < GetInstanceCount(); ++i)
	{
		FTransform InstanceTransform;
		GetInstanceTransform(i, InstanceTransform, true);

		FSwarmInstance* SwarmInstance = new FSwarmInstance;
		SwarmInstance->Velocity = InstanceTransform.GetRotation().GetForwardVector() * SwarmSpeed;
		SwarmInstance->Index = i;
		SwarmInstance->Location = InstanceTransform.GetLocation();

		Instances.Add(SwarmInstance);
	}
}

void USwarmInstancedComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (int i = 0; i < Instances.Num(); ++i)
	{
		delete Instances.Pop();
	}
}

#include "DrawDebugHelpers.h"


void USwarmInstancedComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	static FRotator Rotator;
	Rotator.Yaw += 1000.0f * DeltaTime;
	FVector RotatedVec = Rotator.RotateVector(FVector(1, 0, 0));

	//Optimize this code by partial partition with 3D Grid
	//Get the nearby Instances foreach Instance
	for (int i = 0; i < Instances.Num(); ++i)
	{
		FSwarmInstance* SwarmInstance = Instances[i];
		SwarmInstance->FriendInstances.Empty();

		FVector InstanceLocation = SwarmInstance->Location;
		for (int j = 0; j < GetInstanceCount(); ++j)
		{
			if (i != j)
			{
				FSwarmInstance* OtherSwarmInstance = Instances[j];
				FVector OtherInstanceLocation = OtherSwarmInstance->Location;
				if (IsCollision(InstanceLocation, OtherInstanceLocation))
				{
					SwarmInstance->FriendInstances.Add(OtherSwarmInstance);
				}
			}
		}
	}

	int Factor = 1;

	FlushPersistentDebugLines(GetWorld());
	float Index = 0.0f;
	for (int i = 0; i < Instances.Num(); ++i)
	{
		FSwarmInstance* SwarmInstance = Instances[i];

		float DistanceToTarget = (SwarmInstance->Location - GetComponentLocation()).Size();

		//This line did not work because the operation was (1 - x) / 2000.0f instead of 1 - (x/2000). weird
		//float InstanceSpeed = (1 - (FMath::Clamp(DistanceToTarget, 0.0f, 1500.0f) / 2000.f) * SwarmSpeed);

		float ClampedDistance = FMath::Clamp(DistanceToTarget, 0.0f, 5000.0f);
		float Alpha = ClampedDistance / 10000.0f;

		float InstanceSpeed = (1 - Alpha) * SwarmSpeed;
		InstanceSpeed = FMath::Clamp(InstanceSpeed, SwarmSpeed * 0.75f, SwarmSpeed);
		if (bRotateAroundTarget)
		{
			float SphereRadius = InstanceRadius;

			//Get the scaled direction between the origin and the instance
			FVector DiffVector = SwarmInstance->Location - GetComponentLocation();

			//Get the current radius or length of the scaled direction
			float CurrentRadius = DiffVector.Size();

			//Get the Point on the sphere we want to have
			FVector PointOnSphere = GetComponentLocation() + DiffVector.GetSafeNormal() * SphereRadius;
			
			//Project the Velocity Vector from the point to the sphere
			FVector ProjectionVector = PointOnSphere + SwarmInstance->Velocity;
			ProjectionVector = GetComponentLocation() + (ProjectionVector - GetComponentLocation()).GetSafeNormal() * SphereRadius;


			//Direction from the instances current location to the point on the sphere
			FVector DirectionToSphereOverlay = (PointOnSphere - SwarmInstance->Location);

			SwarmInstance->Velocity = ((DirectionToSphereOverlay.GetSafeNormal() + (ProjectionVector - PointOnSphere).GetSafeNormal()) * 0.5f) * SwarmSpeed;// (DirectionToSphereOverlay.GetSafeNormal() * 100.0f + (ProjectionVector - PointOnSphere)*50.0f).GetSafeNormal() * SwarmSpeed;
				
			if ((SwarmInstance->Location - PointOnSphere).Size() < 10.0f)
			{
				SwarmInstance->Location = PointOnSphere + (ProjectionVector - PointOnSphere) * DeltaTime;
			}
			else
			{
				SwarmInstance->Location += SwarmInstance->Velocity * DeltaTime;
			}
		}
		else
		{
			FlockInstance(SwarmInstance, SwarmSpeed/*InstanceSpeed*/ /*((SwarmSpeed/(Index + 1)) * Factor)*/, SwarmInstance->Location);
		}

		FTransform NewTransform(SwarmInstance->Velocity.GetSafeNormal().ToOrientationQuat(), SwarmInstance->Location, FVector(1.0f));

		UpdateInstanceTransform(SwarmInstance->Index, NewTransform, true, true, true);
	}
}

void USwarmInstancedComponent::RotateInstanceAroundTarget(FSwarmInstance* Instance)
{

}

void USwarmInstancedComponent::FlockInstance(FSwarmInstance* Instance, const float& InstanceSpeed, const FVector& InstanceLocation)
{
	FVector Rule1 = SeperationWeight * SeperateInstance(Instance, InstanceLocation);
	FVector Rule2 = AlignWeight * AlignInstance(Instance, InstanceLocation);
	FVector Rule3 = CohereWeight * CohereInstance(Instance, InstanceLocation);

	FVector NewVelocity = Instance->Velocity + Rule1 + Rule2 + Rule3;

	float Speed = 1.0f;
	FVector VelocityToTarget = FVector::ZeroVector;
	
	{
		FVector Diff = this->GetComponentLocation() - Instance->Location;// (Target->GetActorLocation() - Instance->Location);
		VelocityToTarget += (Diff/100.0f) * TargetWeight;
	}

	NewVelocity += VelocityToTarget;

	if (NewVelocity.ContainsNaN()) NewVelocity = Instance->Velocity + VelocityToTarget;

	if (InstanceSpeed < 0) UE_LOG(LogTemp, Error, TEXT("%f"), InstanceSpeed);

	float DeltaTime = FApp::GetDeltaTime();
	Instance->Velocity = InstanceSpeed * NewVelocity.GetSafeNormal();
	Instance->Location += Instance->Velocity * DeltaTime;
}

FVector USwarmInstancedComponent::AlignInstance(FSwarmInstance* Instance, const FVector& InstanceLocation)
{
	FVector c = FVector::ZeroVector;

	int Count = 0;
	for (int i = 0; i < Instance->FriendInstances.Num(); ++i)
	{
		c += Instance->FriendInstances[i]->Location;
		++Count;
	}

	if (Count > 0)
	{
		c /= Count;
	}

	return (c - Instance->Location);
}

FVector USwarmInstancedComponent::SeperateInstance(FSwarmInstance* Instance, const FVector& InstanceLocation)
{
	FVector c = FVector::ZeroVector;

	int Count = 0;
	for (int i = 0; i < Instance->FriendInstances.Num(); ++i)
	{
		FVector DiffVector = Instance->FriendInstances[i]->Location - InstanceLocation;
		if (DiffVector.Size() < SeperationRadius)
		{
			c -= DiffVector;
			++Count;
		}
	}

	if (Count > 0)
	{
		c /= Count;
	}

	return c;
}

FVector USwarmInstancedComponent::CohereInstance(FSwarmInstance* Instance, const FVector& InstanceLocation)
{
	FVector c = FVector::ZeroVector;

	int Count = Instance->FriendInstances.Num();
	for (int i = 0; i < Count; ++i)
	{
		c += Instance->FriendInstances[i]->Velocity;
	}

	if (Count > 0)
	{
		c /= Count;
	}

	return (c - Instance->Velocity) / 8;
}

bool USwarmInstancedComponent::IsCollision(FVector FirstLocation, FVector SecondLocation)
{
	return (SecondLocation - FirstLocation).Size() <= InstanceRadius * 2;
}

void USwarmInstancedComponent::MergeSwarm(USwarmInstancedComponent* OtherSwarm)
{
	//Only merge swarm if the static mesh components are the same.
	//If there are not, we have to add a new InstancedStaticMesh to the ActorOwner...
	if (GetStaticMesh() == OtherSwarm->GetStaticMesh())
	{
		USwarmInstancedComponent* MergeFrom = OtherSwarm;
		USwarmInstancedComponent* MergeInto = this;

		int MergeFromInstanceCount = OtherSwarm->GetInstanceCount();
		int MergeIntoInstanceCount = GetInstanceCount();

		if (MergeIntoInstanceCount < MergeFromInstanceCount)
		{
			MergeFrom = this;
			MergeInto = OtherSwarm;

			MergeFromInstanceCount = MergeIntoInstanceCount;
		}

		//Easy method: just remove from one instance and add new with same transform to other
		//Difficult method: Get the pointers from MergeFrom and add them to MergeInto.(Look into InstancedStaticMeshComponent.h/.cpp)
		for (int i = MergeFromInstanceCount - 1; i >= 0; --i)
		{
			FTransform Transform;
			MergeFrom->GetInstanceTransform(i, Transform, true);

			MergeFrom->RemoveInstance(i);
			MergeInto->AddInstanceWorldSpace(Transform);

		}
	}
}

void USwarmInstancedComponent::SetProperties(float NewSpeed, float NewTargetWeight, float NewSeperationWeight)
{
	SwarmSpeed = NewSpeed;
	TargetWeight = NewTargetWeight;
	SeperationWeight = NewSeperationWeight;
}

void USwarmInstancedComponent::SetSwarmSpeed(float NewSpeed)
{
	SwarmSpeed = NewSpeed;
}

void USwarmInstancedComponent::SetTargetWeight(float NewWeight)
{
	TargetWeight = NewWeight;
}

void USwarmInstancedComponent::SetSeperationWeight(float NewWeight)
{
	SeperationWeight = NewWeight;
}

float USwarmInstancedComponent::GetSwarmSpeed()
{
	return SwarmSpeed;
}

float USwarmInstancedComponent::GetTargetWeight()
{
	return TargetWeight;
}

float USwarmInstancedComponent::GetSeperationWeight()
{
	return SeperationWeight;
}

void USwarmInstancedComponent::SetFormation(EFormation Formation)
{
	switch (Formation)
	{
		case EFormation::SPHERE:
			bRotateAroundTarget = true;
			break;
		case EFormation::ROTATION:
			bRotateAroundTarget = true;
			break;
		case EFormation::BEE:
			bRotateAroundTarget = false;
	}
}