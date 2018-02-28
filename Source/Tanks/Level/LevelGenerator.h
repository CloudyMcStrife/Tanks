// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"

#include "LevelGenerator.generated.h"

class FIntersectionHelper
{
public:
	static bool LineCircleIntersection(const FVector& LineStart, const FVector& LineEnd, const FVector& CircleOrigin, const float& CircleRadius)
	{
		FVector LineDir = (LineEnd - LineStart).GetSafeNormal();
		FVector LineStartToCircleOrigin = CircleOrigin - LineStart;

		//Project Distance vector from linestart to circle origin to the direction vector from linestart to lineend
		FVector ProjectionVec = FVector::DotProduct(LineDir, LineStartToCircleOrigin) * LineDir;

		float Distance = (CircleOrigin - ProjectionVec).Size();

		//Check if the distance to the projection vector is smaller then the radius
		return Distance <= CircleRadius;
	}

	static bool RectangleCircleIntersection(const FVector RectangleOrigin, const float& RectWidth, const float& RectLength, const FVector& CircleOrigin, const float& CircleRadius)
	{
		FVector CircleDistance;
		CircleDistance.X = FMath::Abs(CircleOrigin.X - RectangleOrigin.X);
		CircleDistance.Y = FMath::Abs(CircleOrigin.Y - RectangleOrigin.Y);

		//Check if the Circle bounds cannot overlap with rectangle
		if (CircleDistance.X > (RectWidth / 2 + CircleRadius)) { return false; }
		if (CircleDistance.Y > (RectLength / 2 + CircleRadius)) { return false; }

		//Check if CircleDistance is smaller then rectangle bounds in x and y axis
		if (CircleDistance.X <= (RectWidth / 2)) { return true; }
		if (CircleDistance.Y <= (RectLength / 2)) { return true; }

		float CornerDistanceSquared = FMath::Pow((CircleDistance.Y - RectWidth / 2), 2) + FMath::Pow((CircleDistance.X - RectLength / 2), 2);

		return (CornerDistanceSquared <= (CircleRadius * CircleRadius));
	}
};

class FSpatialBase
{
public:
	FVector Location;
	float Width, Length;

public:
	FSpatialBase()
		: Location(FVector::ZeroVector), Width(0.0f), Length(0.0f)
	{

	}

	FSpatialBase(const FVector& Location, const float& Width, const float& Length)
		: Location(Location), Width(Width), Length(Length)
	{

	}

	void GetPoints(FVector& TopLeft, FVector& BottomRight) const
	{
		FVector DiagonalVec = FVector(Width, -Length, 0.0f) * 0.5f;

		TopLeft = Location - DiagonalVec;
		BottomRight = Location + DiagonalVec;
	}

	virtual void Draw(UWorld* WorldContext, FColor Color) const
	{
		FVector DrawLocation = Location;
		DrawLocation.Z = 200.0f;

		DrawDebugBox(WorldContext, DrawLocation, FVector(Width, Length, 1) * 0.5f, Color, true, 1.0f, 0, 5.0f);
	}
};

class FSpatialInstance : public FSpatialBase
{
	friend class FSpacialGrid;

public:
	FSpatialInstance(const FVector& Location, const float& _Width, const float& _Length)
	{
		this->Location = Location;
		this->Width = _Width;
		this->Length = _Length;
	}

	bool IsOverlapping(const FVector& OtherLocation, const float& Radius) const
	{
		return FIntersectionHelper::RectangleCircleIntersection(Location, Width, Length, OtherLocation, Radius);
	}

	bool IsOverlapping(const FVector& OtherLocation, const float& OtherWidth, const float& OtherLength) const
	{
		FVector TopLeft, BottomRight;
		GetPoints(TopLeft, BottomRight);

		FVector DiagonalVec = FVector(OtherWidth, -OtherLength, 0.0f) * 0.5f;
		FVector OtherTopLeft = OtherLocation - DiagonalVec;
		FVector OtherBottomRight = OtherLocation + DiagonalVec;

		if ((TopLeft.X > OtherBottomRight.X) || (OtherTopLeft.X > BottomRight.X))
		{
			return false;
		}

		if ((TopLeft.Y < OtherBottomRight.Y) || (OtherTopLeft.Y < BottomRight.Y))
		{
			return false;
		}

		return true;
	}

	bool IsOverlapping(const FSpatialInstance& Other) const
	{
		return IsOverlapping(Other.Location, Other.Width, Other.Length);
	}
};

class FSpatialCell : public FSpatialBase
{
	friend class FSpatialGrid;

protected:
	TArray<FSpatialInstance*> Instances;

public:

	FSpatialCell()
		: FSpatialBase(FVector::ZeroVector, 0.0f, 0.0f)
	{

	}

	FSpatialCell(const FVector& Location, const float& Width, const float& Length)
		: FSpatialBase(Location, Width, Length)
	{

	}

	~FSpatialCell()
	{
		while (Instances.Num() > 0)
		{
			delete Instances.Pop();
		}
	}

	void Clear()
	{
		while (Instances.Num() > 0)
		{
			delete Instances.Pop();
		}
	}

	virtual void Draw(UWorld* WorldContext, FColor Color) const override
	{
		FSpatialBase::Draw(WorldContext, Color);

		for (int i = 0; i < Instances.Num(); ++i)
		{
			Instances[i]->Draw(WorldContext, FColor::Blue);
		}
	}

	bool CanAdd(const FVector& Location, const float& Width, const float& Length) const
	{
		for (int i = 0; i < Instances.Num(); ++i)
		{
			if (Instances[i]->IsOverlapping(Location, Width, Length))
			{
				return false;
			}
		}

		return true;
	}

	bool CanAdd(const FVector& Location, const float& Radius) const
	{
		for (int i = 0; i < Instances.Num(); ++i)
		{
			if (Instances[i]->IsOverlapping(Location, Radius))
			{
				return false;
			}
		}

		return true;
	}

protected:
	FSpatialInstance* Add(const FVector& Location, const float& Width, const float& Length)
	{
		FSpatialInstance* AddInstance = new FSpatialInstance(Location - FVector(0,0,Location.Z), Width, Length);
		Instances.Add(AddInstance);

		return AddInstance;
	}

	bool Add(FSpatialInstance* Instance)
	{
		Instances.Add(Instance);
	}
};

class FSpatialGrid : public FSpatialBase
{
public:
	const uint8 Rows = 5;
	const uint8 Columns = 5;

	mutable TArray<FVector> Circles;

	FSpatialGrid(uint8 _Rows, uint8 _Columns, const float& _Width, const float& _Length, const FVector& _Location)
		: Rows(_Rows), Columns(_Columns)
	{
		Cells = new FSpatialCell*[Rows];

		Width = _Width;
		Length = _Length;
		Location = _Location;

		FVector TopLeft, BottomRight;
		GetPoints(TopLeft, BottomRight);

		float CellWidth = Width / Columns;
		float CellLength = Length / Rows;

		for (int i = 0; i < Rows; ++i) {
			Cells[i] = new FSpatialCell[Columns];
			for (int j = 0; j < Columns; ++j)
			{
				//FVector CellLocation = TopLeft + FVector((0.5f * j))
				Cells[i][j] = FSpatialCell(TopLeft + FVector((0.5f + j) * CellWidth, -(0.5f + i) * CellLength, 0.0f), CellWidth, CellLength);
			}
		}

	}

	~FSpatialGrid()
	{
		for (int i = 0; i < Rows; ++i)
		{
			delete [] Cells[i];
		}

		delete [] Cells;
	}

	virtual void Draw(UWorld* WorldContext, FColor Color) const override
	{
		FSpatialBase::Draw(WorldContext, Color);

		for (int i = 0; i < Rows; ++i) {
			for (int j = 0; j < Columns; ++j) {
				const FSpatialCell& Cell = Cells[i][j];
				Cell.Draw(WorldContext, FColor::Yellow);
			}
		}

		for (int i = 0; i < Circles.Num(); ++i)
		{
			DrawDebugSphere(WorldContext, Circles[i], 50.0f, 32.0f, FColor::Purple, true, 1.0f, 0, 5.0f);
		}
	}

	void ClearGrid()
	{
		for (int i = 0; i < Rows; ++i)
		{
			for (int j = 0; j < Columns; ++j)
			{
				Cells[i][j].Clear();
			}
		}
	}

	bool IsFreeSpaceForSphere(FSpatialCell* Cell, int cellX, int cellY, const FVector& Location, const float& Radius) const
	{
		//for (int i = 0; i < Rows; ++i)
		{
			//for (int j = 0; j < Columns; ++j)
			{
				if (!Cell->CanAdd(Location, Radius)) return false;
			}
		}

		return true;
	}

	bool IsFreeSpaceForSphere(int cellX, int cellY, const FVector& Location, const float& Radius) const
	{
		FVector UniLocation = Location;
		UniLocation.Z = 0.0f;

		if (!GetCell(cellX, cellY)) return false;

		for (int i = 0; i < Rows; ++i)
		{
			for (int j = 0; j < Columns; ++j)
			{
				if (FSpatialCell* Cell = GetCell((cellX - 1) + i, (cellY - 1) + j))
				{
					if (!Cell->CanAdd(UniLocation, Radius)) return false;
				}
			}
		}

		return true;
	}

	FVector GetFreeLocation_Sphere(const float& Radius) const
	{
		FVector DiagonalVec = FVector(Width, Length, 0) * 0.5f;

		FVector ReturnVector;

		bool bFoundLocation = false;
		while (!bFoundLocation)
		{
			FVector RandomPoint = FMath::RandPointInBox(FBox(Location + DiagonalVec, Location - DiagonalVec));

			int cellX, cellY;
			if (GetCell(RandomPoint, cellX, cellY))
			{
				if (IsFreeSpaceForSphere(cellX, cellY, RandomPoint, Radius))
				{
					bFoundLocation = true;
					ReturnVector = RandomPoint;
				}
			}
		}

		FVector DrawCircleLocation = ReturnVector;
		DrawCircleLocation.Z = 200.0f;

		if (Circles.Num() == 2)
		{
			Circles[0] = Circles[1];
			Circles[1] = DrawCircleLocation;
		}
		else
		{
			Circles.Add(DrawCircleLocation);
		}

		return ReturnVector;
	}

	FSpatialCell* GetCell(const FVector& Location, int& cellX, int& cellY) const
	{
		FVector TopLeft = this->Location - FVector(this->Width * 0.5f, this->Length * 0.5f, 0);

		float a = (-TopLeft.X + Location.X) / (this->Width);
		float b = (-TopLeft.Y + Location.Y) / (this->Length);

		cellX = (int)(a * Columns);
		cellY = (int)(b * Rows);

		return GetCell(cellX, cellY);
	}

	FSpatialCell* GetCell(int cellX, int cellY) const
	{
		if (cellX >= 0 && cellX < Columns && cellY >= 0 && cellY < Rows)
		{
			return &Cells[cellX][cellY];
		}

		return nullptr;
	}

	bool CanAdd(FSpatialCell* Cell, int cellX, int cellY, FVector Location, const float& Width, const float& Height)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				if (FSpatialCell* c = GetCell((cellX - 1) + i, (cellY - 1) + j))
				{
					if (!c->CanAdd(Location, Width, Height)) return false;
				}
			}
		}

		return true;
	}

	FSpatialInstance* Add(const FVector& Location, const float& Width, const float& Length)
	{
		int cellX, cellY;
		if (FSpatialCell* Cell = GetCell(Location, cellX, cellY))
		{
			if (CanAdd(Cell, cellX, cellY, Location, Width, Length))
			{
				return Cell->Add(Location, Width, Length);
			}
		}
		
		return nullptr;
	}

private:
	FSpatialCell** Cells;
};


UCLASS()
class TANKS_API ALevelGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

	FVector GetFreeLocation_Sphere(float Radius);
	FVector GetFreeLocation_Box(float Width, float Length);

	virtual void OnConstruction(const FTransform& Transform) override;

	void GenerateBlockables();
	void GenerateBoundaries();

	//Boundary Height
	UPROPERTY(EditAnywhere, Category = "Level|Procedural")
		float Height;

	//Width in Unreal Units
	UPROPERTY(EditAnywhere, Category = "Level|Procedural")
		float Width;

	//Height in Unreal Units
	UPROPERTY(EditAnywhere, Category = "Level|Procedural")
		float Length;

	UPROPERTY(EditAnywhere, Category = "Level|Procedural")
		bool bGenerate;

	UPROPERTY(EditAnywhere, Category = "Level|Procedural", meta = (ClampMin = "0", ClampMax = "20"))
		int MaxBlockadeCount;
	
	UPROPERTY(EditAnywhere, Category = "Level|Ground")
		UStaticMeshComponent* GroundMesh;

	UPROPERTY(EditAnywhere, Category = "Level|Procedural")
		class UInstancedStaticMeshComponent* BlockableInstance;

	FSpatialGrid* Grid;
};
