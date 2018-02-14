// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorPool.h"

#include "Tanks.h"

ActorPool* ActorPool::Instance = nullptr;

ActorPool::ActorPool()
	: MaxSize(20), Count(0)
{
}

ActorPool::~ActorPool()
{
	DeleteResources();
}

void ActorPool::ReleaseMap(TMap<TSubclassOf<AActor>, TArray<AActor*>>& Map)
{
	for (auto Iterator = Map.CreateIterator(); Iterator; ++Iterator)
	{
		TArray<AActor*>& Arr = Iterator.Value();

		while (Arr.Num() > 0)
		{
			Arr.Pop()->Destroy();
		}
	}
}

void ActorPool::DeleteResources()
{
	ReleaseMap(Resources);
	ReleaseMap(ResourcesInUse);
}

ActorPool* ActorPool::GetInstance()
{
	if (Instance) return Instance;

	Instance = new ActorPool();
	return Instance;
}

void ActorPool::DeleteInstance()
{
	delete Instance;

	Instance = nullptr;
}

TWeakObjectPtr<AActor> ActorPool::GetResource(UWorld* WorldContext, TSubclassOf<AActor> Subclass)
{
	if (Count < MaxSize)
	{
		TArray<AActor*>& Arr = Resources.FindOrAdd(Subclass);

		//If the Array is empty we create a new Actor and spawn it
		AActor* Resource = (Arr.Num() == 0) ? WorldContext->SpawnActor<AActor>(Subclass) : Arr.Pop();
		ResourcesInUse.FindOrAdd(Subclass).Add(Resource);

		++Count;
		FActorHelper::EnableActor(*Resource);

		return Resource;
	}

	return nullptr;
}

void ActorPool::ReturnResource(AActor* Actor)
{
	if (Actor)
	{
		if (TArray<AActor*>* Arr = ResourcesInUse.Find(Actor->GetClass()))
		{
			if (Arr->Remove(Actor) != 0)
			{
				Resources.FindOrAdd(Actor->GetClass()).Add(Actor);

				FActorHelper::EnableActor(*Actor, false);
				--Count;
			}
		}
	}
}

void ActorPool::SetMaxSize(uint8 NewSize)
{
	MaxSize = NewSize;
}