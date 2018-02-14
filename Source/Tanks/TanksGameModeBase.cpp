// Fill out your copyright notice in the Description page of Project Settings.

#include "TanksGameModeBase.h"

#include "Controller/TankController.h"


ATanksGameModeBase::ATanksGameModeBase()
	: Super()
{
	PlayerControllerClass = ATankController::StaticClass();
}
