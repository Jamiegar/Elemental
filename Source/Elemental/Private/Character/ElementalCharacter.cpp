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
	CurrentNumDashes = _numOfDashes;
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
	static_cast<UCharacterCurveMovementComponent*>(GetCharacterMovement())->AddCurveForwardBackMovement(GetActorForwardVector(), Axis);
}

void AElementalCharacter::MoveRight(const float Axis)
{
	static_cast<UCharacterCurveMovementComponent*>(GetCharacterMovement())->AddCurveRightLeftMovement(GetActorRightVector(), Axis);
}

void AElementalCharacter::CharJump()
{
	Jump();
}

void AElementalCharacter::Dash()
{
	if(CurrentNumDashes <= 0 || CanDash == false)
		return;

	const FVector Vel = GetVelocity();
	const FVector DashVel = FVector(Vel.X, Vel.Y, 0) * _dashForceMultiplier;
	LaunchCharacter(DashVel, false, false);
	CurrentNumDashes--;

	if(CurrentNumDashes <= 0)
		CanDash = false;
	
	GetWorldTimerManager().SetTimer(CoolDownTimerHandle, this, &AElementalCharacter::ResetDashes, _dashCoolDown, false);
}

void AElementalCharacter::ResetDashes()
{
	UE_LOG(LogTemp, Warning, TEXT("IsOnGround: %s"), GetMovementComponent()->IsFalling()? TEXT("false") : TEXT("true"));
	
    if(CoolDownTimerHandle.IsValid())
        CoolDownTimerHandle.Invalidate();
	
	if(GetMovementComponent()->IsFalling())
	{
		return;
	}

	CurrentNumDashes = _numOfDashes;
	CanDash = true;
}

void AElementalCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if(CanDash == false && !CoolDownTimerHandle.IsValid())
		ResetDashes();
}