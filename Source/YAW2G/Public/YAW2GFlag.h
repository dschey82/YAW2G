// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "YAW2GFlag.generated.h"

class ATriggerVolume;

UENUM()
enum class ECaptureState : uint8
{
	Uncaptured,
	Allied,
	Axis,
	CapturingAxis2Allied,
	CapturingAllied2Axis,
	CapturingNeutral2Axis,
	CapturingNeutral2Allied
};

UCLASS()
class YAW2G_API AYAW2GFlag : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly)
	ATriggerVolume * FlagTrigger = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ATriggerVolume> TriggerVolumeClass;
	
public:	
	// Sets default values for this actor's properties
	AYAW2GFlag();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
