// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ElementalCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/CharacterCurveMovementComponent.h"


// Sets default values
AElementalCharacter::AElementalCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterCurveMovementComponent>(CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
}

float AElementalCharacter::CalcTotalKnockback(const float Damage)
{
	_totalKnockBackPercentage += Damage;
	return _totalKnockBackPercentage;
}

void AElementalCharacter::ApplyKnockback(const float Damage, AActor* Inst)
{
	FVector direction = GetActorLocation() - Inst->GetActorLocation(); //Gets the direction away from the actor dealing damage
	direction.Normalize();

	const FVector launchVel = direction * Damage * _totalKnockBackPercentage;
	LaunchCharacter(launchVel, false, false);
}

void AElementalCharacter::MoveForward(const float Axis)
{
	if (_canMove == false)
		return;

	static_cast<UCharacterCurveMovementComponent*>(GetCharacterMovement())->AddCurveForwardBackMovement(GetActorForwardVector(), Axis);
	_movementYAxis = Axis;
}

void AElementalCharacter::MoveRight(const float Axis)
{
	if (_canMove == false)
		return;

	static_cast<UCharacterCurveMovementComponent*>(GetCharacterMovement())->AddCurveRightLeftMovement(GetActorRightVector(), Axis);
	_movementXAxis = Axis;
}

void AElementalCharacter::CharJump()
{
	Jump();
}

