// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "YAW2GFlag.h"
#include "Yaw2gGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateLocalFlags);

/**
 * 
 */
UCLASS()
class YAW2G_API AYaw2gGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
	void PostInitializeComponents() override;

	FTimerHandle TimerHandle_TrackFlagCap;

public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly)
	TArray<AYAW2GFlag *> FlagArray;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FlagState)
	TArray<EFlagState> FlagStateArray;

	UFUNCTION()
	void UpdateFlagStateArray();

	UFUNCTION()
	void OnRep_FlagState();

	int AxisPoints;
	int AlliedPoints;
	
	UPROPERTY(BlueprintAssignable)
	FUpdateLocalFlags UpdateLocalFlags;
};
