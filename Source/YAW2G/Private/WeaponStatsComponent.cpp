// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponStatsComponent.h"
#include "InventoryComponent.h"


// Sets default values for this component's properties
UWeaponStatsComponent::UWeaponStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


FSWeaponStats UWeaponStatsComponent::GetWeaponStats() const
{
	FSWeaponStats ReturnStats = {};
	ReturnStats.Name = name;
	ReturnStats.FireDelay = fireDelay;
	ReturnStats.ReloadDelay = reloadDelay;
	ReturnStats.MaxLoadedAmmo = maxLoadedAmmo;
	ReturnStats.DamageAmount = damageAmount;
	ReturnStats.WeaponSlot = weaponSlot;
	return ReturnStats;
}

void UWeaponStatsComponent::SetWeaponStats(FSWeaponStats NewStats)
{
	name = NewStats.Name;
	fireDelay = NewStats.FireDelay;
	reloadDelay = NewStats.ReloadDelay;
	maxLoadedAmmo = NewStats.MaxLoadedAmmo;
	weaponSlot = NewStats.WeaponSlot;
	damageAmount = NewStats.DamageAmount;
}

int UWeaponStatsComponent::GetLoadedAmmo() const
{
	return loadedAmmo;
}

int UWeaponStatsComponent::GetMaxLoadedAmmo() const
{
	return maxLoadedAmmo;
}

int UWeaponStatsComponent::GetDamageAmount() const
{
	return damageAmount;
}

void UWeaponStatsComponent::DecrementLoadedAmmo()
{
	--loadedAmmo;
}

void UWeaponStatsComponent::ReloadWeapon(int AmmoAmount)
{	
	loadedAmmo += AmmoAmount;
}
