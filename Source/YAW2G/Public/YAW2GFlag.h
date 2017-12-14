// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "YAW2GFlag.generated.h"

class UCapsuleComponent;

UENUM()
enum class EFlagState : uint8
{	
	Allied,
	Axis,
	Neutral,
	Capturing,
};

UENUM()
enum class ECaptureState : uint8
{
	Allied,
	Axis,
	None,
};

UCLASS()
class YAW2G_API AYAW2GFlag : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly)
	UCapsuleComponent * CapsuleComponent = nullptr;

	EFlagState CurrentFlagState = EFlagState::Neutral;
	ECaptureState CurrentCaptureState = ECaptureState::None;

	FTimerHandle TimerHandle_FlagCapture;
	
public:	
	// Sets default values for this actor's properties
	AYAW2GFlag();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetFlagStateAxis();
	void SetFlagStateAllied();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
