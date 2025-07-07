// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TPSCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class TPS_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATPSCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void MouseLook(const FInputActionValue& Value);
	void JumpAction(const FInputActionValue& Value);

private:

	//Camera
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	//Input
	// Input Mapping Contexts
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* IMC_Default;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* IMC_MouseLook;

	// Input Actions
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Jump;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Move;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Look;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MouseLook;

public:	

};
