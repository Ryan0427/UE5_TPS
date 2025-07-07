// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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
}

void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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


void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
