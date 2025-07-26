// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSAnimInstance.h"
#include "TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UTPSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	TPSCharacter = Cast<ATPSCharacter>(TryGetPawnOwner());
}

void UTPSAnimInstance::NativeUpdateAnimation(float Deltatime)
{
	Super::NativeUpdateAnimation(Deltatime);

	if (TPSCharacter == nullptr)
	{
		TPSCharacter = Cast<ATPSCharacter>(TryGetPawnOwner());
	}

	if (TPSCharacter == nullptr) return;

	FVector Velocity = TPSCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = TPSCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = TPSCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = TPSCharacter->IsWeaponEquipped();
	bIsCrouched = TPSCharacter->bIsCrouched;
}
