// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "YAW2GGameMode.h"
#include "YAW2GHUD.h"
#include "YAW2GCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "GameFramework/GameState.h"
#include "MyPlayerStart.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"

AYAW2GGameMode::AYAW2GGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AYAW2GHUD::StaticClass();

	PlayerControllerClass = AMyPlayerController::StaticClass();

	PlayerStateClass = AMyPlayerState::StaticClass();
}

void AYAW2GGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		AMyPlayerState * PS = Cast<AMyPlayerState>(NewPlayer->PlayerState);
		if (PS && GameState)
		{
			uint8 numTeamAllies = 0;
			uint8 numTeamAxis = 0;

			for (APlayerState * It : GameState->PlayerArray)
			{
				AMyPlayerState* OtherPS = Cast<AMyPlayerState>(It);
				if (OtherPS)
				{
					if (OtherPS->bTeamAxis)
					{
						++numTeamAxis;
					}
					else
					{
						++numTeamAllies;
					}
				}
			}
			if (numTeamAllies > numTeamAxis)
			{
				PS->bTeamAxis = true;
			}
			else 
			{
				PS->bTeamAxis = false;
			}			
		}
	}
}

AActor * AYAW2GGameMode::ChoosePlayerStart_Implementation(AController * Player)
{
	if (Player)
	{
		AMyPlayerState * PS = Cast<AMyPlayerState>(Player->PlayerState);
		if (PS)
		{
			TArray<AMyPlayerStart *> Starts;
			for (TActorIterator<AMyPlayerStart> StartItr(GetWorld()); StartItr; ++StartItr)
			{
				if (StartItr->bTeamAxis == PS->bTeamAxis)
				{					
					Starts.Add(*StartItr);
				}
			}
			int randomPick = FMath::RandRange(0, Starts.Num() - 1);
			return Starts[randomPick];
		}
	}
	return NULL;
}

bool AYAW2GGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}