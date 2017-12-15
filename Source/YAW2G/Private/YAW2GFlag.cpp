// Fill out your copyright notice in the Description page of Project Settings.

#include "YAW2GFlag.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "YAW2GCharacter.h"
#include "MyPlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameMode.h"


// Sets default values
AYAW2GFlag::AYAW2GFlag()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->bGenerateOverlapEvents = true;
	CapsuleComponent->SetCapsuleRadius(200.0f);
	CapsuleComponent->SetCapsuleHalfHeight(300.0f);
	CapsuleComponent->bHiddenInGame = false;
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AYAW2GFlag::OnOverlapBegin);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &AYAW2GFlag::OnOverlapEnd);

}

// Called when the game starts or when spawned
void AYAW2GFlag::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AYAW2GFlag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AYAW2GFlag::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check state of flag capture & compare to team ownership of triggering player
	// If player's team matches the current flag captured status, then do nothing
	if ((uint8)Cast<AMyPlayerState>(Cast<AYAW2GCharacter>(OtherActor)->PlayerState)->bTeamAxis == (uint8)CurrentFlagState) return;

	// Check to see team ownership of all actors currently overlapping
	TSet<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AYAW2GCharacter::StaticClass());
	int AxisActors = 0;
	int AlliedActors = 0;

	for (auto actor : OverlappingActors)
	{
		if (Cast<AMyPlayerState>(Cast<AYAW2GCharacter>(actor)->PlayerState)->bTeamAxis) ++AxisActors;
		else ++AlliedActors;
	}
	
	if (AxisActors > 0 && AlliedActors == 0)
	{
		CurrentCaptureState = ECaptureState::Axis;
		GetWorldTimerManager().SetTimer(TimerHandle_FlagCapture, this, &AYAW2GFlag::SetFlagStateAxis, 3.0f);
	}
	else if (AxisActors == 0 && AlliedActors > 0)
	{
		CurrentCaptureState = ECaptureState::Allied;
		GetWorldTimerManager().SetTimer(TimerHandle_FlagCapture, this, &AYAW2GFlag::SetFlagStateAllied, 3.0f);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_FlagCapture);
		return;
	}	
}

void AYAW2GFlag::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TSet<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AYAW2GCharacter::StaticClass());
	int AxisActors = 0;
	int AlliedActors = 0;

	for (auto actor : OverlappingActors)
	{
		if (Cast<AMyPlayerState>(Cast<AYAW2GCharacter>(actor)->PlayerState)->bTeamAxis) ++AxisActors;
		else ++AlliedActors;
	}

	if (AxisActors > 0 && AlliedActors == 0)
	{
		CurrentCaptureState = ECaptureState::Axis;
		GetWorldTimerManager().SetTimer(TimerHandle_FlagCapture, this, &AYAW2GFlag::SetFlagStateAxis, 3.0f);
	}
	else if (AxisActors == 0 && AlliedActors > 0)
	{
		CurrentCaptureState = ECaptureState::Allied;
		GetWorldTimerManager().SetTimer(TimerHandle_FlagCapture, this, &AYAW2GFlag::SetFlagStateAllied, 3.0f);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_FlagCapture);
		return;
	}
}

void AYAW2GFlag::SetFlagStateAxis()
{
	CurrentFlagState = EFlagState::Axis;
	UE_LOG(LogTemp, Warning, TEXT("Axis Forces have captured the flag!"));
	if (GetNetMode() == NM_DedicatedServer)
	{
		GetWorld()->GetAuthGameMode()->ProcessServerTravel("/Game/FirstPersonCPP/Maps/FirstPersonExampleMap?dedicated", true);
	}
	
}

void AYAW2GFlag::SetFlagStateAllied()
{
	CurrentFlagState = EFlagState::Allied;
	UE_LOG(LogTemp, Warning, TEXT("Allied Forces have captured the flag!"));
	if (GetNetMode() == NM_DedicatedServer)
	{
		GetWorld()->GetAuthGameMode()->ProcessServerTravel("/Game/FirstPersonCPP/Maps/FirstPersonExampleMap?dedicated", true);
	}
}
