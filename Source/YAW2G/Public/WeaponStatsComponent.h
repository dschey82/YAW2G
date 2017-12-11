// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponStatsComponent.generated.h"

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	SevenNineTwo,
	FortyFiveACP,
	ThirtyAughtSix,
	NineMM
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Melee,
	Secondary,
	Primary
};

USTRUCT(BlueprintType)
struct FSWeaponStats
{
	GENERATED_BODY()

	FString Name;
	int FireDelay;
	int ReloadDelay;
	uint16 DamageAmount;
	uint16 MaxLoadedAmmo;
	EWeaponSlot WeaponSlot;
	EAmmoType AmmoType;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YAW2G_API UWeaponStatsComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString name;
	UPROPERTY(EditAnywhere)
	int fireDelay;
	UPROPERTY(EditAnywhere)
	int reloadDelay;
	UPROPERTY(EditAnywhere)
	EWeaponSlot weaponSlot;
	UPROPERTY(EditAnywhere)
	EAmmoType ammoType;
	UPROPERTY(EditAnywhere)
	uint16 maxLoadedAmmo;
	UPROPERTY(EditAnywhere)
	uint16 loadedAmmo;
	UPROPERTY(EditAnywhere)
	uint16 damageAmount;

public:	
	// Sets default values for this component's properties
	UWeaponStatsComponent();

	UFUNCTION(BlueprintCallable)
	FSWeaponStats GetWeaponStats() const;

	UFUNCTION(BlueprintCallable)
	void SetWeaponStats(FSWeaponStats NewStats);

	UFUNCTION(BlueprintCallable)
	int GetLoadedAmmo() const;

	UFUNCTION(BlueprintCallable)
	int GetMaxLoadedAmmo() const;

	UFUNCTION(BlueprintCallable)
	int GetDamageAmount() const;

	void DecrementLoadedAmmo();
	void ReloadWeapon(int AmmoAmount);

protected:
	// Called when the game starts
	// virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
