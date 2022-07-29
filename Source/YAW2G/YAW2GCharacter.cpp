// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "YAW2GCharacter.h"
#include "YAW2GProjectile.h"
#include "XRMotionControllerBase.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "UnrealNetwork.h"
#include "MyPlayerController.h"
#include "InventoryComponent.h"
#include "WeaponStatsComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "YAW2GFlag.h"
#include "Yaw2gGameStateBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AYAW2GCharacter

void AYAW2GCharacter::UpdateFlagElements()
{
	UpdateFlagElementsBP.Broadcast();
}

AYAW2GCharacter::AYAW2GCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	PlayerInventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	PlayerInventoryComponent->InitializePlayerAmmoCount();

	WeaponStatsComponent = CreateDefaultSubobject<UWeaponStatsComponent>(TEXT("WeaponStatsComponent"));

	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	bShouldRegenStamina = true;
	Stamina = 100.0f;
	Health = 100.0f;	
}

void AYAW2GCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AYAW2GCharacter::OnOverlapBegin);

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

	if (Controller) Controller->SetControlRotation(GetActorRotation());

	Cast<AYaw2gGameStateBase>(GetWorld()->GetGameState())->UpdateLocalFlags.AddDynamic(this, &AYAW2GCharacter::UpdateFlagElements);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AYAW2GCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->IsChildOf(AYaw2GWeapon::StaticClass()))
	{
		FSWeaponStats returnStats;
		AYaw2GWeapon * weapon = Cast<AYaw2GWeapon>(OtherActor);
		if (weapon != nullptr)
		{
			CurrentWeaponStats = weapon->GetWeaponStats();
		}
		if(WeaponStatsComponent) WeaponStatsComponent->SetWeaponStats(CurrentWeaponStats);

		PlayerInventoryComponent->IncreaseReserveAmmo(CurrentWeaponStats.AmmoType, 30);
		return;
	}
}

void AYAW2GCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AYAW2GCharacter::StartFiring);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AYAW2GCharacter::StopFiring);

	// Bind reload event
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AYAW2GCharacter::Reload);

	// Bind sprint event
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AYAW2GCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AYAW2GCharacter::StopSprinting);

	// Bind crouch event
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AYAW2GCharacter::StartCrouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AYAW2GCharacter::StopCrouching);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AYAW2GCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AYAW2GCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AYAW2GCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AYAW2GCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AYAW2GCharacter::LookUpAtRate);
}

void AYAW2GCharacter::OnFire()
{
	if (Task != ETaskEnum::Fire) return;			

	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AYAW2GProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetViewRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
				ActorSpawnParams.Instigator = this;

				// spawn the projectile at the muzzle				
				World->SpawnActor<AYAW2GProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
				WeaponStatsComponent->DecrementLoadedAmmo();
				if (WeaponStatsComponent->GetLoadedAmmo() == 0) Task = ETaskEnum::None;
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("StartingTimer for %f"), (float)WeaponStatsComponent->GetWeaponStats().FireDelay / 1000.f);
	GetWorldTimerManager().SetTimer(TimerHandle_Task, this, &AYAW2GCharacter::OnFire, (float)(CurrentWeaponStats.FireDelay / 1000.f));
}

void AYAW2GCharacter::Reload()
{
	currentAmmoLoaded = WeaponStatsComponent->GetLoadedAmmo();
	if (PlayerInventoryComponent->GetReserveAmmoCount(CurrentWeaponStats.AmmoType) > 0 && currentAmmoLoaded < CurrentWeaponStats.MaxLoadedAmmo)
	{
		WeaponStatsComponent->ReloadWeapon(PlayerInventoryComponent->DecreaseReserveAmmo(CurrentWeaponStats.AmmoType, CurrentWeaponStats.MaxLoadedAmmo - currentAmmoLoaded));
		Task = ETaskEnum::Reload;
		GetWorldTimerManager().SetTimer(TimerHandle_Reload, this, &AYAW2GCharacter::EndReload, (float)(CurrentWeaponStats.ReloadDelay / 1000.f));
	}	
}

void AYAW2GCharacter::EndReload()
{
	Task = ETaskEnum::None;
}

void AYAW2GCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AYAW2GCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AYAW2GCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AYAW2GCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AYAW2GCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value, true);		
	}
}

void AYAW2GCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AYAW2GCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AYAW2GCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AYAW2GCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AYAW2GCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AYAW2GCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AYAW2GCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void AYAW2GCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FirstPersonCameraComponent->SetWorldRotation(GetViewRotation());
	//UE_LOG(LogTemp, Warning, TEXT("%s"), bShouldRegenStamina ? TEXT("TRUE") : TEXT("FALSE"));
	if (bShouldRegenStamina && Stamina <= 99.9f)
	{
		Stamina += .1;
	}
}

FRotator AYAW2GCharacter::GetViewRotation() const
{
	if (Controller)
	{
		return Controller->GetControlRotation();
	}
	return FRotator(RemoteViewPitch / 255.f * 360.f, GetActorRotation().Yaw, 0.f);
}

void AYAW2GCharacter::OnRep_Task()
{
	switch (Task)
	{
	case (ETaskEnum::None):
		break;
	case (ETaskEnum::Fire):
		OnFire();
		break;
	case (ETaskEnum::Reload):
		break;
	}	
}

void AYAW2GCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AYAW2GCharacter, Task);
	DOREPLIFETIME(AYAW2GCharacter, Health);
	DOREPLIFETIME(AYAW2GCharacter, Stamina);
}

void AYAW2GCharacter::StartFiring()
{
	// If Reloading, out of ammo, sprinting, or weapon is on firing cooldown, don't fire
	if (	Task == ETaskEnum::Reload
		||	WeaponStatsComponent->GetLoadedAmmo() == 0
		||	!bShouldRegenStamina
		||	(TimerHandle_Task.IsValid() && GetWorldTimerManager().GetTimerRemaining(TimerHandle_Task) > 0.f)
		)	return;

	PerformTask(ETaskEnum::Fire);
}

void AYAW2GCharacter::StopFiring()
{
	PerformTask(ETaskEnum::None);
}

void AYAW2GCharacter::StartSprinting()
{
	UpdateWalkSpeed(1200.f, false);
}

void AYAW2GCharacter::StopSprinting()
{
	UpdateWalkSpeed(600.f, true);	
}

void AYAW2GCharacter::UpdateWalkSpeed(float NewWalkSpeed, bool ShouldRegen)
{
	if (GetNetMode() == NM_Client)
	{	
		ServerUpdateWalkSpeed(NewWalkSpeed, ShouldRegen);
		return;
	}

	// Listenserver only
	GetCharacterMovement()->MaxWalkSpeed = NewWalkSpeed;
	bShouldRegenStamina = ShouldRegen;
	GetWorldTimerManager().ClearTimer(TimerHandle_StaminaSprintLoop);
	if (!ShouldRegen) Task = ETaskEnum::None;	
}

void AYAW2GCharacter::ServerUpdateWalkSpeed_Implementation(float NewWalkSpeed, bool ShouldRegen)
{	
	GetCharacterMovement()->MaxWalkSpeed = NewWalkSpeed;
	bShouldRegenStamina = ShouldRegen;
	if (ShouldRegen)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_StaminaSprintLoop);
	}
	else
	{
		GetWorldTimerManager().SetTimer(TimerHandle_StaminaSprintLoop, this, &AYAW2GCharacter::ReduceStaminaByOne, .05, true, .05);
		Task = ETaskEnum::None;
	}
}

bool AYAW2GCharacter::ServerUpdateWalkSpeed_Validate(float NewWalkSpeed, bool ShouldRegen)
{
	return true;
}

void AYAW2GCharacter::StartCrouching()
{
	GetCharacterMovement()->bWantsToCrouch = true;
}

void AYAW2GCharacter::StopCrouching()
{
	GetCharacterMovement()->bWantsToCrouch = false;
}

void AYAW2GCharacter::PerformTask(ETaskEnum::Type NewTask)
{
	if (GetNetMode() == NM_Client)
	{		
		ServerPerformTask(NewTask);
		return;
	}

	// Listenserver only
	Task = NewTask;
	OnRep_Task();
}

void AYAW2GCharacter::ServerPerformTask_Implementation(ETaskEnum::Type NewTask)
{
	Task = NewTask;
	OnRep_Task();
}

bool AYAW2GCharacter::ServerPerformTask_Validate(ETaskEnum::Type NewTask)
{
	return true;
}

float AYAW2GCharacter::GetStaminaPercent() const
{
	return Stamina / 100.f;
}

float AYAW2GCharacter::TakeDamage(float DamageAmount,struct FDamageEvent const & DamageEvent,class AController * EventInstigator,AActor * DamageCauser)
{
	Health -= DamageAmount;
	
	if (Health <= 0.f)
	{
		AMyPlayerController * PC = Cast<AMyPlayerController>(Controller);
		if (PC)
		{
			PC->OnKilled();
		}

		Destroy();
	}

	OnRep_Health();
	return DamageAmount;
}

void AYAW2GCharacter::OnRep_Health()
{

}

void AYAW2GCharacter::OnRep_Stamina()
{

}

void AYAW2GCharacter::ReduceStaminaByOne()
{
	if (--Stamina < 30.0f) StopSprinting();
}

int AYAW2GCharacter::GetAmmoCountBP() const
{
	return WeaponStatsComponent->GetLoadedAmmo();
}

int AYAW2GCharacter::GetReserveAmmoCountBP() const
{
	return PlayerInventoryComponent->GetReserveAmmoCount(CurrentWeaponStats.AmmoType);
}

float AYAW2GCharacter::GetReloadTimerPercentBP() const
{
	return (float)(GetWorldTimerManager().GetTimerRemaining(TimerHandle_Reload) * 1000 / CurrentWeaponStats.ReloadDelay) ; // Reload Delay is stored in ms
}

float AYAW2GCharacter::GetFlagCapProgress(AYAW2GFlag * pFlag) const
{
	return pFlag->GetFlagCaptureProgress();
}

void AYAW2GCharacter::EventTestFunc()
{
	
}

float AYAW2GCharacter::GetHealthPercent() const
{
	return Health / 100.f;
}