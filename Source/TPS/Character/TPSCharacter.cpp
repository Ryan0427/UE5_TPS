// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "TPS/Weapon/Weapon.h"
#include "TPS/TPSComponent/CombatComponent.h"

ATPSCharacter::ATPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void ATPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
}

void ATPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATPSCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Cast to EnhancedInputComponent
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("EnhancedInputComponent not found! Ensure project is configured for Enhanced Input."));
		return;
	}

	// Add Input Mapping Contexts
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Add IMC_Default with lower priority (0)
			if (IMC_Default)
			{
				Subsystem->AddMappingContext(IMC_Default, 0);
			}
			// Add IMC_MouseLook with higher priority (1)
			if (IMC_MouseLook)
			{
				Subsystem->AddMappingContext(IMC_MouseLook, 1);
			}
		}
	}

	// Bind Input Actions
	if (IA_Jump)
	{
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ATPSCharacter::JumpAction);
	}
	if (IA_Move)
	{
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ATPSCharacter::Move);
	}
	if (IA_Look)
	{
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ATPSCharacter::Look);
	}
	if (IA_MouseLook)
	{
		EnhancedInputComponent->BindAction(IA_MouseLook, ETriggerEvent::Triggered, this, &ATPSCharacter::MouseLook);
	}

	if (IA_Equip)
	{
		EnhancedInputComponent->BindAction(IA_Equip, ETriggerEvent::Triggered, this, &ATPSCharacter::EquipAction);
	}

	if (IA_Crouch)
	{
		EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Triggered, this, &ATPSCharacter::CrouchAction);
	}

	if (IA_Aim)
	{
		EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Started, this, &ATPSCharacter::AimActionPressed);
	}

	if (IA_Aim)
	{
		EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Completed, this, &ATPSCharacter::AimActionReleased);
	}
}

void ATPSCharacter::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();

		// Forward/Backward movement (Y axis)
		if (MoveValue.Y != 0.f)
		{
			const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, MoveValue.Y * 2.f);
		}

		// Right/Left movement (X axis)
		if (MoveValue.X != 0.f)
		{
			const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(Direction, MoveValue.X * 2.f);
		}
	}
}

void ATPSCharacter::Look(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D LookValue = Value.Get<FVector2D>();

		// Yaw rotation
		if (LookValue.X != 0.f)
		{
			AddControllerYawInput(LookValue.X);
		}
		// Pitch rotation
		if (LookValue.Y != 0.f)
		{
			AddControllerPitchInput(LookValue.Y);
		}
	}
}

void ATPSCharacter::MouseLook(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D MouseLookValue = Value.Get<FVector2D>();

		// Yaw rotation (horizontal, Mouse X)
		if (MouseLookValue.X != 0.f)
		{
			AddControllerYawInput(MouseLookValue.X);
		}
		// Pitch rotation (vertical, Mouse Y)
		if (MouseLookValue.Y != 0.f)
		{
			AddControllerPitchInput(MouseLookValue.Y);
		}
	}
}

void ATPSCharacter::JumpAction(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		Jump();
	}
}

void ATPSCharacter::EquipAction(const FInputActionValue& Value)
{
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}

		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void ATPSCharacter::CrouchAction(const FInputActionValue& Value)
{
	if (bIsCrouched)
	{
		UnCrouch();
	}

	else
	{
		Crouch();
	}
}

void ATPSCharacter::AimActionPressed(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ATPSCharacter::AimActionReleased(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ATPSCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ATPSCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;

	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void ATPSCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}

	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

bool ATPSCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ATPSCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}
