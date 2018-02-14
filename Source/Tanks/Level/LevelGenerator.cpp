// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelGenerator.h"

#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
ALevelGenerator::ALevelGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	GroundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ground"));
	BlockableInstance = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BlockableInstance"));

	GroundMesh->SetupAttachment(RootComponent);
	BlockableInstance->SetupAttachment(RootComponent);

	bGenerate = false;

	Length = 10.0f;
	Width = 10.0f;
	Height = 5.0f;
}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	if (!Grid)
	{
		Grid = new FSpatialGrid(10, 10, Width * 80.0f, Length * 80.0f, GetActorLocation());

		int32 Count = BlockableInstance->GetInstanceCount();

		if (Count < 4) return;

		for (int i = 4; i < Count; ++i)
		{
			FTransform InstanceTransform;
			BlockableInstance->GetInstanceTransform(i, InstanceTransform, true);

			FVector Location = InstanceTransform.GetLocation();
			float w = InstanceTransform.GetScale3D().X * 100.0f;
			float l = InstanceTransform.GetScale3D().Y * 100.0f;

			Grid->Add(Location, w, l);
		}
	}
}

void ALevelGenerator::EndPlay(const EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

	delete Grid;

	Grid = nullptr;
}

// Called every frame
void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

//#define DEBUG_DRAW
#ifdef DEBUG_DRAW
	FlushPersistentDebugLines(GetWorld());
	Grid->Draw(GetWorld(), FColor::Yellow);
#endif

}

void ALevelGenerator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	GroundMesh->SetWorldScale3D(FVector(Width, Length, 0.1f));

	if (bGenerate)
	{
		BlockableInstance->ClearInstances();

		GenerateBoundaries();
		GenerateBlockables();

		bGenerate = false;
	}
}


void ALevelGenerator::GenerateBlockables()
{
	delete Grid;
	Grid = new FSpatialGrid(10, 10, Width * 80.0f, Length * 80.0f, GetActorLocation());
	Grid->ClearGrid();

	for (int i = 0; i < MaxBlockadeCount; ++i)
	{
		FTransform InstanceTransform;
		BlockableInstance->GetInstanceTransform(i, InstanceTransform, true);

		FVector Vec = FVector(Width * 40.0f, Length * 40.0f, 0);

		FVector RandomPoint = FMath::RandPointInBox(FBox(Vec, -Vec));
		RandomPoint += GetActorLocation();
		
		float w = 100.0f;
		float l = 100.0f;

		if (Grid->Add(RandomPoint, w, l))
		{
			RandomPoint.Z = 50.0f;
			BlockableInstance->AddInstanceWorldSpace(FTransform(RandomPoint));
		}
	}
}

void ALevelGenerator::GenerateBoundaries()
{
	FVector Center = GroundMesh->GetComponentLocation();

	float BoundaryWidth = Width*0.05f;
	float BoundaryLength = Length;

	float Scale = 1.0f;
	for (int i = 0; i < 4; ++i)
	{
		if ((i % 2) == 0)
		{
			FVector BoundaryCenter(Scale * (Width * 0.5f - BoundaryWidth * 0.5f), 0, Height * 0.5f);
			BoundaryCenter *= 100.0f;

			BoundaryCenter += Center;
			
			FVector BoundaryScale(BoundaryWidth, BoundaryLength, Height);
			
			FTransform BoundaryTransform(FRotator::ZeroRotator, BoundaryCenter, BoundaryScale);
			BlockableInstance->AddInstanceWorldSpace(BoundaryTransform);
		}
		else
		{
			FVector BoundaryCenter(0, Scale * (Width * 0.5f - BoundaryWidth * 0.5f), Height * 0.5f);
			BoundaryCenter *= 100.0f;

			BoundaryCenter += Center;

			FVector BoundaryScale(BoundaryLength - BoundaryWidth * 2, BoundaryWidth, Height);

			FTransform BoundaryTransform(FRotator::ZeroRotator, BoundaryCenter, BoundaryScale);
			BlockableInstance->AddInstanceWorldSpace(BoundaryTransform);

			Scale *= -1.0f;
		}

	}
}

FVector ALevelGenerator::GetFreeLocation_Sphere(float Radius)
{
	if (Grid)
	{
		return Grid->GetFreeLocation_Sphere(Radius);
	}

	return FVector::ZeroVector;
}

FVector ALevelGenerator::GetFreeLocation_Box(float Width, float Length)
{
	return FVector::ZeroVector;
}