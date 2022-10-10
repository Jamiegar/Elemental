// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tests/AutomationTestSettings.h"
#include "CharacterCurveMovementComponent.generated.h"

struct CurveValues
{
	float Time;
	float PrevCurveVal;
	float MinCurveTime;
	float MaxCurveTime;

	CurveValues()
	{
		Time = 0.0f;
		PrevCurveVal = 0.0f;
		MinCurveTime = 0.0f;
		MaxCurveTime =0.0f;
	}
	
	CurveValues(const UCurveFloat* Curve)
	{
		Time = 0.0f;
		PrevCurveVal = 0.0f;
		
		if(Curve)
			Curve->GetTimeRange(MinCurveTime, MaxCurveTime);
	}
	
	void SetUpCurveValues(const UCurveFloat* Curve)
	{
		Time = MinCurveTime; //Set the time to the starting value of the curve
		PrevCurveVal = Curve->GetFloatValue(MinCurveTime); //Prev value should be the starting curve value too
	}
};

/**
 * 
 */
UCLASS()
class ELEMENTAL_API UCharacterCurveMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CurveMovement", meta=(DisplayName = "Movement Curve"))
		UCurveFloat* _movementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CurveJump", meta=(DisplayName = "Jump Curve"))
		UCurveFloat* _jumpCurve;

private:
	bool _isJumping = false;
	CurveValues _jumpCurveValues;

	bool _isMoving = false;
	CurveValues _moveCurveValues;

	float _rightAxis;
	float _forwardAxis;
	
	FVector _rightMovementDir;
	FVector _forwardMovementDir;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual bool DoJump(bool bReplayingMoves) override;
	void JumpUpdate(float DeltaTime);
	void FallGroundCheck();

	virtual void AddCurveRightLeftMovement(const FVector MovementDirection, float AxisValue);
	virtual void AddCurveForwardBackMovement(const FVector MovementDirection, float AxisValue);
	void MovementUpdate(float DeltaTime);
	
};
