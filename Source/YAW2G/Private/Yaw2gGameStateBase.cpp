// Fill out your copyright notice in the Description page of Project Settings.

#include "Yaw2gGameStateBase.h"
#include "YAW2GFlag.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealNetwork.h"

void AYaw2gGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AYaw2gGameStateBase, FlagStateArray);
}

void AYaw2gGameStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();	

	TArray<AActor*> OutActors;
	TSubclassOf<AActor> ActorClass = AYAW2GFlag::StaticClass();
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AYAW2GFlag::StaticClass(), OutActors);
	if (true)
	{
		int i = 0;
		for (AActor* pActor : OutActors)
		{	
			AYAW2GFlag* Flag = Cast<AYAW2GFlag>(pActor);
			FlagArray.Add(Flag);
			FlagStateArray.Add(EFlagState::Neutral);
			Flag->OnFlagCapturedEvent.AddDynamic(this, &AYaw2gGameStateBase::UpdateFlagStateArray);
		}
		FlagArray.Sort();
	}
}

void AYaw2gGameStateBase::UpdateFlagStateArray()
{
	for (int i = 0; i < FlagArray.Num(); ++i)
	{
		FlagStateArray[i] = FlagArray[i]->CurrentFlagState;
	}
}

void AYaw2gGameStateBase::OnRep_FlagState()
{
	UpdateLocalFlags.Broadcast();
}


