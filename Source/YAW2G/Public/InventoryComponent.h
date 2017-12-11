// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponStatsComponent.h"
#include "InventoryComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YAW2G_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	TArray<int> reserveAmmoCount;
	TArray<int> maxReserveAmmo;

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	// Set the player's initial ammo count (0) for all types
	// and Max ammo count for all types
	// TODO: pull max ammo counts from blueprint or config
	void InitializePlayerAmmoCount();
	
	UFUNCTION(BlueprintCallable)
	int GetReserveAmmoCount(EAmmoType AmmoType) const;

	void IncreaseReserveAmmo(EAmmoType AmmoType, uint16 PickedUpAmmo);
	int DecreaseReserveAmmo(EAmmoType AmmoType, uint16 ClipSize);
	
};
