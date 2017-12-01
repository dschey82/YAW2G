// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "YAW2GGameMode.h"
#include "YAW2GHUD.h"
#include "YAW2GCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "MyPlayerController.h"

AYAW2GGameMode::AYAW2GGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AYAW2GHUD::StaticClass();

	PlayerControllerClass = AMyPlayerController::StaticClass();
}
