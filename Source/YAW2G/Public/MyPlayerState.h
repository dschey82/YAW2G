// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class YAW2G_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
	void GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const override;

public:
	UPROPERTY(Replicated, EditAnywhere)
	bool bTeamAxis;
};
