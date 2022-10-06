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
	UPROPERTY(EditAnywhere, Category = "Camera")
		class USpringArmComponent* SpringArmComp = nullptr;
	UPROPERTY(EditAnywhere, Category="Camera")
		class UCameraComponent* ThirdPersonCamera = nullptr;

	UPROPERTY(EditAnywhere, Category="Dash")
		float DashForceMultiplier = 10;
	UPROPERTY(EditAnywhere, Category="Dash")
		int NumOfDashes = 1;
	UPROPERTY(EditAnywhere, Category="Dash")
		float DashCoolDown = 1.5;

private:
	int CurrentNumDashes = 0;
	FTimerHandle CoolDownTimerHandle;
	bool CanDash = true;
	
public:
	// Sets default values for this character's properties
	AElementalCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(const float axis);
	void MoveRight(const float axis);
	void Dash();

private:
	void ResetDashes();
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override; 

};
