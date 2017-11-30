// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "Engine/Engine.h"
#include "YAW2GGameMode.h"
#include "UnrealNetwork.h"

void AMyPlayerController::OnKilled()
{
	UnPossess();
	GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &AMyPlayerController::Respawn, 3.0f);
}

void AMyPlayerController::Respawn()
{
	AGameModeBase * GameMode = GetWorld()->GetAuthGameMode();
	if (GameMode)
	{
		APawn * NewPawn = GameMode->SpawnDefaultPawnFor(this, GameMode->ChoosePlayerStart(this));
		Possess(NewPawn);
	}
}

