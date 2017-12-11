// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "WeaponStatsComponent.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UInventoryComponent::InitializePlayerAmmoCount()
{
	reserveAmmoCount.Init(0, 4);
	maxReserveAmmo.Init(90, 4);
}

void UInventoryComponent::IncreaseReserveAmmo(EAmmoType AmmoType, uint16 PickedUpAmmo)
{
	/// TODO: Replace with Clamp function
	if (reserveAmmoCount[(uint8)AmmoType] + PickedUpAmmo <= maxReserveAmmo[(uint8)AmmoType])
	{
		reserveAmmoCount[(uint8)AmmoType] += PickedUpAmmo;
	}
	else {
		reserveAmmoCount[(uint8)AmmoType] = maxReserveAmmo[(uint8)AmmoType];
	}
	///
}

int UInventoryComponent::GetReserveAmmoCount(EAmmoType AmmoType) const
{
	return reserveAmmoCount[(uint8)AmmoType];
}

int UInventoryComponent::DecreaseReserveAmmo(EAmmoType AmmoType, uint16 ClipSize)
{
	// Can't load more than we have in reserve
	int AmmoAmount = FMath::Clamp<int>(ClipSize, 0, reserveAmmoCount[(uint8)AmmoType]);

	reserveAmmoCount[(uint8)AmmoType] -= AmmoAmount;
	return AmmoAmount;
}