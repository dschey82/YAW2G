// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponStatsComponent.h"
#include "GameFramework/Actor.h"
#include "Yaw2GWeapon.generated.h"

class UBoxComponent;
class USkeletalMeshComponent;

UCLASS()
class YAW2G_API AYaw2GWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UWeaponStatsComponent* WSComp;
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetWeaponStats(FSWeaponStats NewStats);


public:	
	// Sets default values for this actor's properties
	AYaw2GWeapon();

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent * Mesh3P;

	UPROPERTY(EditAnywhere)
	UBoxComponent * BoxComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	FSWeaponStats GetWeaponStats() const;
	
};
