// Fill out your copyright notice in the Description page of Project Settings.

#include "Tanks.h"
#include "Modules/ModuleManager.h"
#include "GameFramework/Actor.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Tanks, "Tanks" );

void FActorHelper::EnableActor(AActor& Actor, bool bEnable)
{
	Actor.SetActorEnableCollision(bEnable);
	Actor.SetActorHiddenInGame(!bEnable);
	Actor.SetActorTickEnabled(bEnable);
}