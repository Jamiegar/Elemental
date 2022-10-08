// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ElementalCharacter.generated.h"


UCLASS()
class ELEMENTAL_API AElementalCharacter : public ACharacter
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category ="Camera")
		class USpringArmComponent* _springArmComp = nullptr;
	UPROPERTY(EditAnywhere, Category="Camera")
		class UCameraComponent* _thirdPersonCamera = nullptr;
	UPROPERTY(EditAnywhere, Category="Camera")
		float _cameraRotationSpeed = 0.5;
	UPROPERTY(EditAnywhere, Category="Camera")
		FVector2D _cameraPitchClamp = FVector2D(-137, 0);

	UPROPERTY(EditAnywhere, Category="Dash")
		float _dashForceMultiplier = 10;
	UPROPERTY(EditAnywhere, Category="Dash")
		int _numOfDashes = 1;
	UPROPERTY(EditAnywhere, Category="Dash")
		float _dashCoolDown = 1.5;

private:
	int CurrentNumDashes = 0;
	FTimerHandle CoolDownTimerHandle;
	bool CanDash = true;
	
public:
	// Sets default values for this character's properties
	AElementalCharacter(const FObjectInitializer& ObjectInitializer);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(const float Axis);
	void MoveRight(const float Axis);
	void CharJump();
	void TurnCamera(const float Axis);
	void CameraUp(const float Axis);
	void Dash();

private:
	void ResetDashes();
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override; 

};
