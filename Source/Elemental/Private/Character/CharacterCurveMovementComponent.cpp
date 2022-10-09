// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/CharacterCurveMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"


void UCharacterCurveMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	//Set default values of curve in Curve Values Constructor 
	_jumpCurveValues = CurveValues(_jumpCurve); 
	_moveRightLeftCurveValues = CurveValues(_movementCurve);
	_moveForwardBackCurveValues = CurveValues(_movementCurve);

	MaxWalkSpeed = _movementCurve->GetFloatValue(_moveRightLeftCurveValues.MaxCurveTime);
}

void UCharacterCurveMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	JumpUpdate(DeltaTime);
	
}

void UCharacterCurveMovementComponent::AddCurveRightLeftMovement(const FVector MovementDirection, float AxisValue)
{
	if(!_movementCurve)
		return;

	AxisValue = FMath::Clamp(AxisValue, 0.0f, 1.0f);
	
	if(AxisValue == 0)
	{
		_isMoving =false;
		_moveRightLeftCurveValues.SetUpCurveValues(_movementCurve);
		return;
	}
	
	
	_isMoving = true;
	movementVel = FVector::Zero();
	MovementUpdate(FApp::GetDeltaTime(), MovementDirection, AxisValue, &_moveRightLeftCurveValues);
	Velocity = movementVel;
}

void UCharacterCurveMovementComponent::AddCurveForwardBackMovement(const FVector MovementDirection, float AxisValue)
{
	if(!_movementCurve)
		return;

	AxisValue = FMath::Clamp(AxisValue, 0.0f, 1.0f);
	
	if(AxisValue == 0)
	{
		_isMoving =false;
		_moveForwardBackCurveValues.SetUpCurveValues(_movementCurve);
		return;
	}
	
	_isMoving = true;
	movementVel = FVector::Zero();
	MovementUpdate(FApp::GetDeltaTime(), MovementDirection, AxisValue, &_moveForwardBackCurveValues);
	Velocity = movementVel;
}

void UCharacterCurveMovementComponent::MovementUpdate(float DeltaTime, const FVector MovementDirection, const float Axis, CurveValues* Curves)
{
	if(_isMoving && _movementCurve)
	{
		Curves->Time += DeltaTime;
		if(Curves->Time <= Curves->MaxCurveTime)
		{
			float moveCurveVal = _movementCurve->GetFloatValue(Curves->Time);
			float moveAccelerationCurve = moveCurveVal - Curves->PrevCurveVal;
			Curves->PrevCurveVal = moveCurveVal;
			
			movementVel += moveAccelerationCurve / DeltaTime  * (MovementDirection * Axis);
		}
		else
		{
			CharacterOwner->AddMovementInput(MovementDirection, Axis);
			UE_LOG(LogTemp, Warning, TEXT("Velocity: %f"), MaxWalkSpeed);
		}
	}
}

bool UCharacterCurveMovementComponent::DoJump(bool bReplayingMoves)
{
	if(CharacterOwner && CharacterOwner->CanJump())
	{
		if(!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
		{
			if(_jumpCurve)
			{
				SetMovementMode(MOVE_Falling);
				//Init jump variables in jump function as the jump starts 
				_isJumping = true;
				_jumpCurveValues.SetUpCurveValues(_jumpCurve); //Values are set to starting values
				UE_LOG(LogTemp, Warning, TEXT("Jumpped"));
				return true;
			}
			else
			{
				return Super::DoJump(bReplayingMoves);
			}
		}
	}
	return false;
}

void UCharacterCurveMovementComponent::JumpUpdate(float DeltaTime)
{
	if(_isJumping && _jumpCurve)
	{
		_jumpCurveValues.Time += DeltaTime;

		if(_jumpCurveValues.Time <= _jumpCurveValues.MaxCurveTime)
		{
			float jumpCurveVal = _jumpCurve->GetFloatValue(_jumpCurveValues.Time);
			float jumpCurveValDelta = jumpCurveVal - _jumpCurveValues.PrevCurveVal;
			_jumpCurveValues.PrevCurveVal = jumpCurveVal;

			Velocity.Z = jumpCurveValDelta / DeltaTime;
			const FVector location = GetActorLocation();
			FVector jumpDestination = location + FVector(0.0f, 0.0f, jumpCurveValDelta);

			//Check for collisions above character
			if(Velocity.Z > 0.0f)
			{
				FCollisionQueryParams roofCollisionPrams;
				roofCollisionPrams.AddIgnoredActor(CharacterOwner);
				FCollisionShape capsule = FCollisionShape::MakeCapsule(CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(),
					CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

				FHitResult hit;
				bool isBlocking = GetWorld()->SweepSingleByProfile(hit,location, jumpDestination, CharacterOwner->GetActorRotation().Quaternion(),
					CharacterOwner->GetCapsuleComponent()->GetCollisionProfileName(), capsule, roofCollisionPrams);

				if(isBlocking)
				{
					_isJumping = false;
					CharacterOwner->ResetJumpState();
					Velocity.Z = 0.0f;
					jumpDestination = location;
				}
			}
			
			FLatentActionInfo latentInfo;
			latentInfo.CallbackTarget = this;

			UKismetSystemLibrary::MoveComponentTo((USceneComponent*)CharacterOwner->GetCapsuleComponent(), jumpDestination, CharacterOwner->GetActorRotation(),
												  false, false, 0.0f, true, EMoveComponentAction::Type::Move, latentInfo);
		}
		else
		{
			_isJumping = false;
			CharacterOwner->ResetJumpState();
			SetMovementMode(MOVE_Walking);
		}
	}
}


