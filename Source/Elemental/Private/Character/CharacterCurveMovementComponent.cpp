// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/CharacterCurveMovementComponent.h"

#include <string>

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"


void UCharacterCurveMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	//Set default values of curve in Curve Values Constructor 
	_jumpCurveValues = CurveValues(_jumpCurve); 
	_moveCurveValues = CurveValues(_movementCurve);
	
	MaxWalkSpeed = _movementCurve->GetFloatValue(_moveCurveValues.MaxCurveTime);
}

void UCharacterCurveMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	JumpUpdate(DeltaTime);
	MovementUpdate(DeltaTime);
}


void UCharacterCurveMovementComponent::AddCurveRightLeftMovement(const FVector MovementDirection, float AxisValue)
{
	if(!_movementCurve)
	{
		GetCharacterOwner()->AddMovementInput(MovementDirection, AxisValue);
		return;
	}

	AxisValue = FMath::Clamp(AxisValue, -1.0f, 1.0f);
	
	_isMoving = true;
	_rightAxis = AxisValue;
	_rightMovementDir = MovementDirection;
}

void UCharacterCurveMovementComponent::AddCurveForwardBackMovement(const FVector MovementDirection, float AxisValue)
{
	if(!_movementCurve) //Check Movement Curve is valid
	{
		//If no movement curve implement normal movement
		GetCharacterOwner()->AddMovementInput(MovementDirection, AxisValue);  
		return;
	}

	AxisValue = FMath::Clamp(AxisValue, -1.0f, 1.0f); //Make sure axis value is clamped
	
	_isMoving = true;
	_forwardAxis = AxisValue;
	_forwardMovementDir = MovementDirection;
}

void UCharacterCurveMovementComponent::MovementUpdate(float DeltaTime)
{
	if(_forwardAxis == 0 && _rightAxis == 0)
	{
		_isMoving =false;
		_moveCurveValues.SetUpCurveValues(_movementCurve);
		return;
	}

	if(_isMoving && _movementCurve && MovementMode == MOVE_Walking)
	{
		//Add delta time to movement curve time to get the time the player has been pressing input
		_moveCurveValues.Time += DeltaTime;

		//Check that input has not been held for max curve time
		if(_moveCurveValues.Time <= _moveCurveValues.MaxCurveTime) 
		{
			//Sample Curve at current time input has been held for (returns velocity at that time)
			const float moveCurveVal = _movementCurve->GetFloatValue(_moveCurveValues.Time);
	
			//Calculate the difference in speed to get the acceleration this frame 
			const float moveAccelerationCurve = moveCurveVal - _moveCurveValues.PrevCurveVal;
			_moveCurveValues.PrevCurveVal = moveCurveVal;
			
			
			GetCharacterOwner()->AddMovementInput(_forwardMovementDir, _forwardAxis * moveAccelerationCurve);
			GetCharacterOwner()->AddMovementInput(_rightMovementDir, _rightAxis * moveAccelerationCurve);
			
		}
		else
		{
			GetCharacterOwner()->AddMovementInput(_forwardMovementDir, _forwardAxis);
			GetCharacterOwner()->AddMovementInput(_rightMovementDir, _rightAxis);
		}
	}
	else if(_isMoving && _movementCurve && (MovementMode == MOVE_Flying || MovementMode == MOVE_Falling))
	{
		GetCharacterOwner()->AddMovementInput(_forwardMovementDir, _forwardAxis);
		GetCharacterOwner()->AddMovementInput(_rightMovementDir, _rightAxis);
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
				OnJumpStarted();
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

			UKismetSystemLibrary::MoveComponentTo(CharacterOwner->GetCapsuleComponent(), jumpDestination, CharacterOwner->GetActorRotation(),
												  false, false, 0.0f, true, EMoveComponentAction::Type::Move, latentInfo);

			FallGroundCheck();
		}
		else
		{
			FallGroundCheck();
		}
	}
}

void UCharacterCurveMovementComponent::FallGroundCheck()
{
	if(Velocity.Z < 0.0f)
	{
		const FVector capsuleLocation = UpdatedComponent->GetComponentLocation();
		FFindFloorResult floorHit;
		FindFloor(capsuleLocation, floorHit, false);

		if(floorHit.IsWalkableFloor() && IsValidLandingSpot(capsuleLocation, floorHit.HitResult))
		{
			SetMovementMode(MOVE_Walking);
			CharacterOwner->ResetJumpState();
			_isJumping = false;
		}
		else
		{
			SetMovementMode(MOVE_Falling);
		}
	}
}
