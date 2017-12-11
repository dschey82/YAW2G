// Fill out your copyright notice in the Description page of Project Settings.

#include "Yaw2GWeapon.h"
#include "WeaponStatsComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
AYaw2GWeapon::AYaw2GWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxComponent);

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh3P"));
	Mesh3P->AttachToComponent(BoxComponent, FAttachmentTransformRules::KeepRelativeTransform);

	WSComp = CreateDefaultSubobject<UWeaponStatsComponent>(TEXT("WeaponStatsComponent"));	
}

// Called when the game starts or when spawned
void AYaw2GWeapon::BeginPlay()
{
	Super::BeginPlay();	
}

FSWeaponStats AYaw2GWeapon::GetWeaponStats() const
{
	return WSComp->GetWeaponStats();
}

void AYaw2GWeapon::SetWeaponStats(FSWeaponStats NewStats)
{
	WSComp->SetWeaponStats(NewStats);	
}
