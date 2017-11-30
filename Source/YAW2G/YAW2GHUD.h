// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "YAW2GHUD.generated.h"

UCLASS()
class AYAW2GHUD : public AHUD
{
	GENERATED_BODY()

public:
	AYAW2GHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

