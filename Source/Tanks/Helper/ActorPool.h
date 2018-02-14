// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Map.h"

/**
 * 
 */
class TANKS_API ActorPool
{
private:
	ActorPool();

	static ActorPool* Instance;
	
	uint8 MaxSize;
	uint8 Count;

	//I make a map of actors with Class keys so I can choose which Subclass I can get as a resource
	TMap<TSubclassOf<AActor>, TArray<AActor*>> Resources;
	TMap<TSubclassOf<AActor>, TArray<AActor*>> ResourcesInUse;

	void DeleteResources();

	void ReleaseMap(TMap<TSubclassOf<AActor>, TArray<AActor*>>& Map);
public:
	~ActorPool();

	static ActorPool* GetInstance();

	//We need the WorldContext because we immediately spawn the actor when the resources are empty
	TWeakObjectPtr<AActor> GetResource(UWorld* WorldContext, TSubclassOf<AActor> Subclass);

	void ReturnResource(AActor* Actor);

	static void DeleteInstance();

	void SetMaxSize(uint8 NewSize);

};
